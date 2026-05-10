module;
#include <memory>
#include <nel/3d/mesh_base.h>
#include <nel/3d/mesh.h>
#include <nel/3d/vertex_buffer.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>

module nel_tools.usd.usd_to_mesh.convert.Converter;
import nel_tools.usd.convert;
import nel_tools.usd.format;


namespace nel_tools::usd::usd_to_mesh::convert
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;
    using namespace pxr;

    UsdGeomMesh findMesh(auto stage)
    {
        auto prim = stage->GetDefaultPrim();
        for ( auto child : stage->Traverse())
        {
            if (auto found = UsdGeokmMesh(child))
            {
                return found;
            }

        }
        return UsdGeomMesh(prim);
    }

    auto Converter::convert() -> unique_ptr<IShape>
    {
        UsdGeomMesh mesh = findMesh(stage);
        if (!mesh)
        {
            return nullptr;
        }

        auto target = make_unique<CMesh>();
        CMeshBase::CMeshBaseBuild buildBaseMesh;
        // todo implement mesh base build
        // @see CExportNel::buildBaseMeshInterface (buildBaseMesh, maxBaseBuild, node, time, nodeMatrix);

        // todo implement mesh build
        CMesh::CMeshBuild buildMesh;
        buildMesh.VertexFlags = CVertexBuffer::PositionFlag | CVertexBuffer::NormalFlag;
        //@see CExportNel::buildMeshInterface (*tri, buildMesh, buildBaseMesh, maxBaseBuild, node, time, nodeMap);

        target->build(buildBaseMesh, buildMesh);
        //target->optimizeMaterialUsage();
        return target;
    }
}
