module;
#include <memory>
#include <nel/3d/mesh_base.h>
#include <nel/3d/mesh.h>

module nel_tools.usd.usd_to_mesh.convert.Converter;
import nel_tools.usd.convert;
import nel_tools.usd.format;


namespace nel_tools::usd::usd_to_mesh::convert
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;

    auto Converter::convert() -> unique_ptr<IShape>
    {

        auto target = make_unique<CMesh>();
        CMeshBase::CMeshBaseBuild buildBaseMesh;
        // todo implement mesh base build
        // @see CExportNel::buildBaseMeshInterface (buildBaseMesh, maxBaseBuild, node, time, nodeMatrix);

        // todo implement mesh build
        CMesh::CMeshBuild buildMesh;
        //@see CExportNel::buildMeshInterface (*tri, buildMesh, buildBaseMesh, maxBaseBuild, node, time, nodeMap);

        target->build(buildBaseMesh, buildMesh);
        //target->optimizeMaterialUsage();
        return target;
    }
}
