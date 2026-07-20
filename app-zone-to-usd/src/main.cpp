#include <fmt/color.h>
#include <nel/3d/landscape.h>
#include <nel/3d/zone.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <pxr/usd/usd/stage.h>


import nel_tools.usd.zone_to_usd.Settings;

using namespace nel_tools::usd::zone_to_usd;

void loadInto(NL3D::CZone& target, const std::string& path);

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

        NL3D::CZone loadingZone;
        loadInto(loadingZone, settings.input);
        const auto zoneId(loadingZone.getZoneId());
        NL3D::CLandscape foo; // TODO: causes segfautl, investigate

        pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(settings.output);
        if (!stage)
        {
            fmt::print(fg(fmt::terminal_color::red), "Failed to create stage at {}\n", settings.output);
            return EXIT_FAILURE;
        }

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
        fmt::print(fg(fmt::terminal_color::red), "Error: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

void loadInto(NL3D::CZone& target, const std::string& path)
{
    NLMISC::CIFile file(path);
    target.serial(file);
}
