#include <memory>
#include <fmt/color.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/scene.h>
#include <nel/3d/shape.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <nel/misc/o_xml.h>
#include <pxr/usd/usd/stage.h>


import nel_tools.usd.usd_to_mesh.convert;
import nel_tools.usd.usd_to_mesh.Settings;

using namespace nel_tools::usd::usd_to_mesh;

std::unique_ptr<NLMISC::COFile> openFile(const std::string& path)
{
    auto file = std::make_unique<NLMISC::COFile>(path);
    if (!file->isOpen() )
    {
        throw std::invalid_argument(fmt::format(fmt::emphasis::bold, "Unable to open file for writing: {}", path));
    }
    return file;
}

void writeFile(const std::string& target, const std::unique_ptr<NL3D::IShape>& source)
{
    auto file = openFile(target);
    NL3D::CShapeStream shapeStream(source.get());
    shapeStream.serial(*file);
}

void writeXml(const std::string& target, const std::unique_ptr<NL3D::IShape>& source)
{
    auto file = openFile(target);
    NL3D::CShapeStream shapeStream(source.get());
    NLMISC::COXml xml;

    xml.init(file.get());
    xml.xmlPush("SHAPE");
    shapeStream.serial(xml);
    xml.xmlPop();
    xml.flush();
}

int main(int argc, char** argv)
{
    fmt::print(fmt::emphasis::bold, "usd-to-mesh:\n");

    try
    {
        NLMISC::CApplicationContext context;
        NLMISC::CCmdArgs args;

        args.addAdditionalArg("input", "Input usd file");
        args.addAdditionalArg("output", "Output shape file");
        args.addArg("", "output-xml", "path", "Output shape file in xml format to <path>");
        args.addArg("", "texture-file-remove-path", "", "remove the path from texture files leaving only the filename");

        if (!args.parse(argc, argv))
        {
            if (args.haveLongArg("version") || args.haveLongArg("help"))
            {
                return EXIT_SUCCESS;
            }
            return EXIT_FAILURE;
        }

        auto settings = Settings::from(args);

        pxr::UsdStageRefPtr source = pxr::UsdStage::Open(settings.input);
        if (!source)
        {
            fmt::print(fg(fmt::terminal_color::red), "Failed to open stage at {}\n", settings.input);
            return EXIT_FAILURE;
        }

        NL3D::registerSerial3d();
        NL3D::CScene::registerBasics();

        auto target = convert::convert(settings, source);
        if (!target)
        {
            fmt::print(fg(fmt::terminal_color::red), "Failed to convert stage at {}\n", settings.input);
            return EXIT_FAILURE;
        }

        writeFile(settings.output, target);

        if (settings.outputXml)
        {
            writeXml(*settings.outputXml, target);
        }

        fmt::print(fg(fmt::terminal_color::green), "Successfully created shape file at {}\n", settings.output);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::terminal_color::red), "Error: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
