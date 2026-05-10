#include <fmt/color.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/scene.h>
#include <nel/3d/shape.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/common.h>
#include <nel/misc/file.h>


import nel_tools.usd.convert.mesh.Converter;
import nel_tools.usd.usd_to_mesh.Settings;

using namespace nel_tools::usd::usd_to_mesh;

int main(int argc, char** argv)
{
    fmt::print(fmt::emphasis::bold, "usd-to-mesh:\n");

    try
    {
        NLMISC::CApplicationContext context;
        NLMISC::CCmdArgs args;

        args.addAdditionalArg("input", "Input shape file");
        args.addAdditionalArg("output", "Output usd file");

        if (!args.parse(argc, argv))
        {
            if (args.haveLongArg("version") || args.haveLongArg("help"))
            {
                return EXIT_SUCCESS;
            }
            return EXIT_FAILURE;
        }

        auto settings = Settings::from(args);

        NL3D::registerSerial3d();
        NL3D::CScene::registerBasics();

        NL3D::IShape* shape = nullptr; // do conversion
        NL3D::CShapeStream shapeStream(shape);
        NLMISC::COFile file(settings.output);
        shapeStream.serial(file);

        fmt::print(fg(fmt::terminal_color::green), "Successfully created shape file at {}\n", settings.output);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::terminal_color::red), "Error converting USD file: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
