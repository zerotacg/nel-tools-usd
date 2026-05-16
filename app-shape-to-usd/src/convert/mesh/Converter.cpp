module;
#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/mesh_multi_lod.h>
#include <nel/3d/water_shape.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/xform.h>

module nel_tools.usd.shape_to_usd.convert.mesh.Converter;
import nel_tools.usd.shape_to_usd.convert.material.TextureSettings;
import nel_tools.usd.shape_to_usd.convert.mesh.ConverterCMesh;
import nel_tools.usd.shape_to_usd.convert.mesh.ConverterCMeshMRMSkinned;
import nel_tools.usd.shape_to_usd.paths;
import nel_tools.usd.shape_to_usd.tokens;

namespace nel_tools::usd::shape_to_usd::convert::mesh
{
    using namespace NL3D;
    using namespace std;
    using namespace pxr;
    using namespace paths;
    using namespace tokens;

    void Converter::convert(const material::TextureSettings& settings, UsdStageRefPtr& target, IShape* shape)
    {
        UsdGeomSetStageUpAxis(target, UsdGeomTokens->z);
        UsdGeomSetStageMetersPerUnit(target, 1.0);
        target->GetRootLayer()->SetDefaultPrim(Tokens.root);
        auto modelRoot = UsdGeomXform::Define(target, Paths.root);
        modelRoot.GetPrim().SetCustomDataByKey(TfToken("nel:class_name"), VtValue(shape->getClassName()));

        if (auto specific = dynamic_cast<CMesh*>(shape))
        {
            std::make_unique<ConverterCMesh>(settings, target, modelRoot, specific)->run();
        }
        if (auto specific = dynamic_cast<CMeshMRM*>(shape))
        {
        }
        if (auto specific = dynamic_cast<CMeshMRMSkinned*>(shape))
        {
            std::make_unique<ConverterCMeshMRMSkinned>(settings, target, modelRoot, specific)->run();
        }
        if (auto specific = dynamic_cast<CMeshMultiLod*>(shape))
        {
        }
        if (auto specific = dynamic_cast<CWaterShape*>(shape))
        {
        }
    }
}
