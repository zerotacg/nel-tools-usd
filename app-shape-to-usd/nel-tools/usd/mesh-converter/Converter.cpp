#include <nel-tools/usd/mesh-converter/Converter.h>

#include <nel/3d/mesh_mrm.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/3d/mesh_multi_lod.h>
#include <nel/3d/skeleton_shape.h>
#include <nel/3d/water_shape.h>

#include <nel-tools/usd/mesh-converter/ConverterCMesh.h>

using namespace NL3D;
using namespace std;

unique_ptr<Converter> Converter::from(IShape *shape, IShape *skeleton)
{
	if (dynamic_cast<CMesh *>(shape))
	{
		return std::make_unique<ConverterCMesh>(dynamic_cast<CMesh *>(shape));
	}
	if (dynamic_cast<CMeshMRM *>(shape))
	{
	}
	if (dynamic_cast<CMeshMRMSkinned *>(shape))
	{
	}
	if (dynamic_cast<CMeshMultiLod *>(shape))
	{
	}
	if (dynamic_cast<CWaterShape *>(shape))
	{
	}

	return std::unique_ptr<Converter>{};
}