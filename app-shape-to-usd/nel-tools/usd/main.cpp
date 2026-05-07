#include <fmt/color.h>
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

int main(int argc, char** argv)
{
    fmt::print(fmt::emphasis::bold, "shape-to-usd:\n");

    try
    {
        NLMISC::CApplicationContext myApplicationContext;
        NLMISC::CCmdArgs args;

        args.addAdditionalArg("input", "Input shape file");
        args.addAdditionalArg("output", "Output usd file");
        if (!args.parse(argc, argv))
        {
            if (args.haveLongArg("version") || args.haveLongArg("help"))
            {
                return EXIT_SUCCESS;
            }
            else
            {
                return EXIT_FAILURE;
            }
        }

        std::string inputFilePath = args.getAdditionalArg("input").front();
        std::string outputFilePath = args.getAdditionalArg("output").front();

        NL3D::registerSerial3d();
        NL3D::CScene::registerBasics();

        NLMISC::CIFile inputFile(inputFilePath);
        NL3D::CShapeStream shapeStream;
        shapeStream.serial(inputFile);
        inputFile.close();
        NL3D::IShape* shape = shapeStream.getShapePointer();
        fmt::print("Shape is of type {}\n", shape->getClassName());

        // Create a new USD stage
        pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(outputFilePath);
        if (!stage)
        {
            fmt::print(fg(fmt::color::red), "Failed to create stage at {}\n", outputFilePath);
            return EXIT_FAILURE;
        }

        Converter::from(stage, shape, nullptr);

        // Save the stage
        if (!stage->GetRootLayer()->Save())
        {
            fmt::print(fg(fmt::color::red), "Failed to save file at {}\n", outputFilePath);
            return EXIT_FAILURE;
        }

        fmt::print(fg(fmt::color::green), "Successfully created USD file at {}\n", outputFilePath);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::color::red), "Error converting shape file: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
