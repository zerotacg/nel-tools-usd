module;
#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/mesh_multi_lod.h>
#include <nel/3d/skeleton_shape.h>
#include <nel/3d/water_shape.h>
#include <pxr/usd/usd/stage.h>

module nel_tools.usd.mesh_converter.Converter;
import nel_tools.usd.mesh_converter.ConverterCMesh;

using namespace NL3D;
using namespace std;
using namespace pxr;

void Converter::convert(const Settings& settings, UsdStageRefPtr& target, IShape* shape)
{
    if (dynamic_cast<CMesh*>(shape))
    {
        std::make_unique<ConverterCMesh>(settings, target, dynamic_cast<CMesh*>(shape))->run();
    }
    if (dynamic_cast<CMeshMRM*>(shape))
    {
    }
    if (dynamic_cast<CMeshMRMSkinned*>(shape))
    {
    }
    if (dynamic_cast<CMeshMultiLod*>(shape))
    {
    }
    if (dynamic_cast<CWaterShape*>(shape))
    {
    }
}
