#include <fmt/color.h>
#include <nel/3d/landscape.h>
#include <nel/3d/zone.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <pxr/usd/usd/stage.h>


import nel_tools.usd.zone_to_usd.Settings;

using namespace nel_tools::usd::zone_to_usd;

int main(int argc, char** argv)
{
    fmt::print(fmt::emphasis::bold, "zone-to-usd:\n");

    try
    {
        NLMISC::CApplicationContext context;
        NLMISC::CCmdArgs args;

        args.addAdditionalArg("input", "Input zone file");
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

        NLMISC::CIFile zoneFile;
        if (!zoneFile.open(settings.input))
        {
            fmt::print(fg(fmt::terminal_color::red), "Can't open the file for reading: {}\n", settings.input);
            return EXIT_FAILURE;
        }
        NL3D::CLandscape landscape;
        NL3D::CZone loadingZone;
        loadingZone.serial(zoneFile);
        zoneFile.close();
        const auto zoneId(loadingZone.getZoneId());
        landscape.setNoiseMode(false);

        // Create a new USD stage
        pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(settings.output);
        if (!stage)
        {
            fmt::print(fg(fmt::terminal_color::red), "Failed to create stage at {}\n", settings.output);
            return EXIT_FAILURE;
        }

        // Save the stage
        if (!stage->GetRootLayer()->Save())
        {
            fmt::print(fg(fmt::terminal_color::red), "Failed to save file at {}\n", settings.output);
            return EXIT_FAILURE;
        }

        fmt::print(fg(fmt::terminal_color::green), "Successfully created USD file at {}\n", settings.output);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::terminal_color::red), "Error converting shape file: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
