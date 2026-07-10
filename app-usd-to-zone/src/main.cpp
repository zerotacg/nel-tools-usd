#include <fmt/color.h>
#include <nel/3d/landscape.h>
#include <nel/3d/zone.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <pxr/usd/usd/stage.h>


import nel_tools.usd.usd_to_zone.Settings;

using namespace nel_tools::usd::usd_to_zone;

std::unique_ptr<NLMISC::COFile> openFile(const std::string& path);

int main(int argc, char** argv)
{
    fmt::print(fmt::emphasis::bold, "usd-to-zone:\n");

    try
    {
        NLMISC::CApplicationContext context;
        NLMISC::CCmdArgs args;

        args.addAdditionalArg("input", "Input usd file");
        args.addAdditionalArg("output", "Output zone file");

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

        auto outputFile = openFile(settings.output);

        NL3D::CZone zone;
        zone.serial(*outputFile);

        fmt::print(fg(fmt::terminal_color::green), "Successfully created zone file at {}\n", settings.output);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::terminal_color::red), "Error converting zone file: {}\n", e.what());
        return EXIT_FAILURE;
    }
}


std::unique_ptr<NLMISC::COFile> openFile(const std::string& path)
{
    auto file = std::make_unique<NLMISC::COFile>(path);
    if (!file->isOpen() )
    {
        throw std::invalid_argument(fmt::format(fmt::emphasis::bold, "Unable to open file for writing: {}", path));
    }
    return file;
}
