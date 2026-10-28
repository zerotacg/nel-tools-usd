#include <ranges>
#include <fmt/color.h>
#include <nel/3d/landscape.h>
#include <nel/3d/zone.h>
#include <nel/misc/app_context.h>
#include <nel/misc/cmd_args.h>
#include <nel/misc/file.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/nurbsPatch.h>


import nel_tools.usd.usd_to_zone.Settings;
import nel_tools.usd.usd_to_zone.tokens;

using namespace nel_tools::usd::usd_to_zone;
using namespace nel_tools::usd::usd_to_zone::tokens;

std::unique_ptr<NLMISC::COFile> openFile(const std::string& path);
auto convert(const pxr::VtArray<pxr::GfVec3f>& source) -> std::vector<NLMISC::CVector>;
auto vertices(const pxr::UsdAttribute& source) -> std::vector<NLMISC::CVector>;
auto convert(const pxr::UsdGeomNurbsPatch& source) -> NL3D::CPatchInfo;
auto isPatch(const pxr::UsdPrim& p) { return p.IsA<pxr::UsdGeomNurbsPatch>(); }

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

        NL3D::CZoneInfo zone_info{
            .ZoneId = 0
        };
        auto prim = source->GetDefaultPrim();
        if (!prim)
        {
            fmt::print(fg(fmt::terminal_color::red), "Failed find default prim\n");
            return EXIT_FAILURE;
        }
        for (auto child : prim.GetChildren() | std::views::filter(isPatch))
        {
            zone_info.Patchs.push_back(convert(pxr::UsdGeomNurbsPatch(child)));
        }


        NL3D::CZone zone;
        zone.build(zone_info);
        zone.serial(*outputFile);

        fmt::print(fg(fmt::terminal_color::green), "Successfully created zone file at {}\n", settings.output);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fmt::print(fg(fmt::terminal_color::red), "Error: {}\n", e.what());
        return EXIT_FAILURE;
    }
}


std::unique_ptr<NLMISC::COFile> openFile(const std::string& path)
{
    auto file = std::make_unique<NLMISC::COFile>(path);
    if (!file->isOpen())
    {
        throw std::invalid_argument(fmt::format(fmt::emphasis::bold, "Unable to open file for writing: {}", path));
    }
    return file;
}

auto vertices(const pxr::UsdAttribute& source) -> std::vector<NLMISC::CVector>
{
    std::vector<NLMISC::CVector> target;
    pxr::VtArray<pxr::GfVec3f> temp;
    source.Get(&temp);

    return convert(temp);
}

auto convert(const pxr::VtArray<pxr::GfVec3f>& source) -> std::vector<NLMISC::CVector>
{
    std::vector<NLMISC::CVector> target;
    target.reserve(source.size());
    for (auto element : source)
    {
        target.emplace_back(element[0], element[1], element[2]);
    }

    return target;
}

auto convert(const pxr::UsdGeomNurbsPatch& source) -> NL3D::CPatchInfo
{
    auto points = vertices(source.GetPointsAttr());

    NL3D::CPatchInfo patch_info;
    patch_info.Patch.Vertices[0] = points[0];
    patch_info.Patch.Vertices[1] = points[12];
    patch_info.Patch.Vertices[2] = points[15];
    patch_info.Patch.Vertices[3] = points[3];
    patch_info.Patch.Tangents[0] = points[4];
    patch_info.Patch.Tangents[1] = points[8];
    patch_info.Patch.Tangents[2] = points[13];
    patch_info.Patch.Tangents[3] = points[14];
    patch_info.Patch.Tangents[4] = points[11];
    patch_info.Patch.Tangents[5] = points[7];
    patch_info.Patch.Tangents[6] = points[2];
    patch_info.Patch.Tangents[7] = points[1];
    patch_info.Patch.Interiors[0] = points[5];
    patch_info.Patch.Interiors[1] = points[9];
    patch_info.Patch.Interiors[2] = points[10];
    patch_info.Patch.Interiors[3] = points[6];
    patch_info.OrderS = 2;
    patch_info.OrderT = 2;
    {
        auto orderSValue = source.GetPrim().GetCustomDataByKey(Tokens.nelOrderS);
        if (orderSValue.IsHolding<uint8>())
        {
            patch_info.OrderS = orderSValue.Get<uint8>();
        }
    }
    {
        auto orderTValue = source.GetPrim().GetCustomDataByKey(Tokens.nelOrderT);
        if (orderTValue.IsHolding<uint8>())
        {
            patch_info.OrderT = orderTValue.Get<uint8>();
        }
    }
    patch_info.Tiles.emplace_back();
    patch_info.Tiles.emplace_back();
    patch_info.Tiles.emplace_back();
    patch_info.Tiles.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();
    patch_info.TileColors.emplace_back();

    return patch_info;
}