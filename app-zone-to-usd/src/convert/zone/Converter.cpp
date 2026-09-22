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

        std::make_unique<Converter>( target, zoneRoot, zone)->run();
    }

    void Converter::run()
    {
        auto outZone = UsdGeomNurbsPatch::Define(stage, Paths.zone);
        for (sint patchIndex = 0; patchIndex < zone->getNumPatchs(); patchIndex++)
        {
        }
        outZone.CreateUOrderAttr().Set(4);
        outZone.CreateVOrderAttr().Set(4);
        outZone.CreateUVertexCountAttr().Set(4);
        outZone.CreateVVertexCountAttr().Set(4);
    }
}
