module;

#include <nel/3d/zone.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/nurbsPatch.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xform.h>

module nel_tools.usd.zone_to_usd.convert.zone.Converter;
import nel_tools.usd.zone_to_usd.paths;
import nel_tools.usd.zone_to_usd.tokens;

namespace nel_tools::usd::zone_to_usd::convert::zone
{
    using namespace NL3D;
    using namespace std;
    using namespace pxr;
    using namespace paths;
    using namespace tokens;

    void Converter::convert(UsdStageRefPtr& target, const CZone* zone)
    {
        UsdGeomSetStageUpAxis(target, UsdGeomTokens->z);
        UsdGeomSetStageMetersPerUnit(target, 1.0);
        target->GetRootLayer()->SetDefaultPrim(Tokens.root);
        auto zoneRoot = UsdGeomXform::Define(target, Paths.root);

        std::make_unique<Converter>(target, zoneRoot, zone)->run();
    }

    void Converter::run()
    {
        auto outZone = UsdGeomXform::Define(stage, Paths.zone);
        for (sint patchIndex = 0; patchIndex < zone->getNumPatchs(); patchIndex++)
        {
            auto outPatch = UsdGeomNurbsPatch::Define(stage, patch(patchIndex));
            convert(outPatch, *zone->getPatch(patchIndex));
        }
    }

    VtArray<GfVec3f> Converter::value(const std::span<CVector>& source)
    {
        VtArray<GfVec3f> target;
        target.reserve(source.size());
        for (auto const& value : source)
        {
            target.emplace_back(value.x, value.y, value.z);
        }

        return target;
    }

    void Converter::convert(UsdGeomNurbsPatch& target, const CPatch& source)
    {
        target.GetPrim().SetCustomDataByKey(Tokens.nelOrderS, VtValue(source.getOrderS()));
        target.GetPrim().SetCustomDataByKey(Tokens.nelOrderT, VtValue(source.getOrderT()));

        CBezierPatch bezierPatch;
        source.unpack(bezierPatch);
        convert(target, bezierPatch);
    }

    void Converter::convert(UsdGeomNurbsPatch& target, const CBezierPatch& source)
    {
        target.CreateOrientationAttr().Set(UsdGeomTokens->leftHanded);
        target.CreateUOrderAttr().Set(4);
        target.CreateVOrderAttr().Set(4);
        target.CreateUVertexCountAttr().Set(4);
        target.CreateVVertexCountAttr().Set(4);
        target.CreateUKnotsAttr().Set(VtArray<double>{0, 0, 0, 0, 1, 1, 1, 1});
        target.CreateVKnotsAttr().Set(VtArray<double>{0, 0, 0, 0, 1, 1, 1, 1});
        target.CreateURangeAttr().Set(GfVec2d(0.0, 1.0));
        target.CreateVRangeAttr().Set(GfVec2d(0.0, 1.0));
        target.CreateUFormAttr().Set(UsdGeomTokens->open);
        target.CreateVFormAttr().Set(UsdGeomTokens->open);

        auto points = std::vector{
            source.Vertices[0], source.Tangents[7], source.Tangents[6], source.Vertices[3],
            source.Tangents[0], source.Interiors[0], source.Interiors[3], source.Tangents[5],
            source.Tangents[1], source.Interiors[1], source.Interiors[2], source.Tangents[4],
            source.Vertices[1], source.Tangents[2], source.Tangents[3], source.Vertices[2]
        };
        target.CreatePointsAttr().Set(value(points));
    }
}
