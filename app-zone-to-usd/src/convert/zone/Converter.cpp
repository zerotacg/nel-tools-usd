module;

#include <nel/3d/zone.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/nurbsPatch.h>
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
        auto outPatch = UsdGeomNurbsPatch::Define(stage, Paths.zone);
        outPatch.CreateUOrderAttr().Set(4);
        outPatch.CreateVOrderAttr().Set(4);
        outPatch.CreateUVertexCountAttr().Set(4);
        outPatch.CreateVVertexCountAttr().Set(4);
        outPatch.CreateUKnotsAttr().Set(VtArray<double>{0, 0, 0, 0, 1, 1, 1, 1});
        outPatch.CreateVKnotsAttr().Set(VtArray<double>{0, 0, 0, 0, 1, 1, 1, 1});
        outPatch.CreateURangeAttr().Set(GfVec2d(0.0, 1.0));
        outPatch.CreateVRangeAttr().Set(GfVec2d(0.0, 1.0));
        outPatch.CreateUFormAttr().Set(Tokens.open);
        outPatch.CreateVFormAttr().Set(Tokens.open);
        for (sint patchIndex = 0; patchIndex < zone->getNumPatchs(); patchIndex++)
        {
            CBezierPatch bezierPatch;
            const CPatch* patch = zone->getPatch(patchIndex);
            patch->unpack(bezierPatch);
            auto points = std::vector{
                bezierPatch.Vertices[0], bezierPatch.Tangents[7], bezierPatch.Tangents[6], bezierPatch.Vertices[3],
                bezierPatch.Tangents[0], bezierPatch.Interiors[0], bezierPatch.Interiors[3], bezierPatch.Tangents[5],
                bezierPatch.Tangents[1], bezierPatch.Interiors[1], bezierPatch.Interiors[2], bezierPatch.Tangents[4],
                bezierPatch.Vertices[1], bezierPatch.Tangents[2], bezierPatch.Tangents[3], bezierPatch.Vertices[2]
            };
            outPatch.CreatePointsAttr().Set(value(points));
        }
    }

    VtArray<GfVec3f> Converter::value(const std::vector<CVector>& source)
    {
        VtArray<GfVec3f> target;
        for (auto const& value : source)
        {
            target.emplace_back(value.x, value.y, value.z);
        }

        return target;
    }
}
