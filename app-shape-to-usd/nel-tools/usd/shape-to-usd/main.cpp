#include <fmt/color.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/ar/defaultResolver.h>
#include <pxr/usd/usd/stage.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/scene.h>
#include <nel/3d/shape.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/common.h>
#include <nel/misc/file.h>

#include <nel-tools/usd/mesh-converter/Converter.h>
#include <nel-tools/usd/shape-to-usd/Settings.h>

using nel_tools::usd::shape_to_usd::Settings;

int main(int argc, char** argv)
{
    fmt::print(fmt::emphasis::bold, "shape-to-usd:\n");

    try
    {
        NLMISC::CApplicationContext context;
        NLMISC::CCmdArgs args;

        args.addAdditionalArg("input", "Input shape file");
        args.addAdditionalArg("output", "Output usd file");
        args.addArg("", "add-search-path", "path", "additional path to search for assets, can be supplied multiple times");
        args.addArg("", "texture-file-to-lower-case", "", "convert texture filename to lower case");
        args.addArg("", "texture-file-replace-extension", "ext", "replace texture file extension with <ext>");

        if (!args.parse(argc, argv))
        {
            if (args.haveLongArg("version") || args.haveLongArg("help"))
            {
                return EXIT_SUCCESS;
            }
            return EXIT_FAILURE;
        }

        auto settings = Settings::from(args);

        pxr::ArDefaultResolver::SetDefaultSearchPath(settings.assets.searchPaths);

        NL3D::registerSerial3d();
        NL3D::CScene::registerBasics();

        NLMISC::CIFile inputFile(settings.input);
        NL3D::CShapeStream shapeStream;
        shapeStream.serial(inputFile);
        inputFile.close();
        NL3D::IShape* shape = shapeStream.getShapePointer();
        fmt::print("Shape is of type {}\n", shape->getClassName());

        // Create a new USD stage
        pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(settings.output);
        if (!stage)
        {
            fmt::print(fg(fmt::color::red), "Failed to create stage at {}\n", settings.output);
            return EXIT_FAILURE;
        }

        Converter::from(settings.texture, stage, shape, nullptr);

        // Save the stage
        if (!stage->GetRootLayer()->Save())
        {
            fmt::print(fg(fmt::color::red), "Failed to save file at {}\n", settings.output);
            return EXIT_FAILURE;
        }

        fmt::print(fg(fmt::color::green), "Successfully created USD file at {}\n", settings.output);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::color::red), "Error converting shape file: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
