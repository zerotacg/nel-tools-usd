module;
#include <memory>
#include <ranges>
#include <vector>
#include <nel/3d/material.h>
#include <nel/3d/mesh_base.h>
#include <nel/3d/mesh.h>
#include <nel/3d/vertex_buffer.h>
#include <nel/misc/vector.h>
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

    UsdGeomMesh findMesh(const auto &stage)
    {
        auto prim = stage->GetDefaultPrim();
        auto isMesh = [](const UsdPrim& p) { return p.IsA<UsdGeomMesh>(); };
        for (auto child : stage->Traverse() | std::views::filter(isMesh))
        {
            return UsdGeomMesh(child);
        }
        return UsdGeomMesh(prim);
    }

    CMaterial defaultMaterial()
    {
        CMaterial material;
        material.initLighted();
        material.setLighting(true, CRGBA::Black, CRGBA::White, CRGBA::White, CRGBA::Black);

        return material;
    }

    vector<CMaterial> buildMaterials(const auto &stage)
    {
        vector<CMaterial> materials;
        materials.push_back(defaultMaterial());

        return materials;
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
        // @see CExportNel::buildMaterials
        buildBaseMesh.Materials = buildMaterials(stage);

        // todo implement mesh build
        CMesh::CMeshBuild buildMesh;
        buildMesh.VertexFlags = CVertexBuffer::PositionFlag | CVertexBuffer::NormalFlag;
        //@see CExportNel::buildMeshInterface (*tri, buildMesh, buildBaseMesh, maxBaseBuild, node, time, nodeMap);
        VtArray<GfVec3f> vertices;
        mesh.GetPointsAttr().Get(&vertices);
        buildMesh.Vertices.resize(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            auto vertex = vertices[i];
            buildMesh.Vertices[i] = CVector(vertex[0], vertex[1], vertex[2]);
        }

        VtArray<int> faceIndices;
        mesh.GetFaceVertexIndicesAttr().Get(&faceIndices);
        auto nNumFaces = mesh.GetFaceCount();
        buildMesh.Faces.resize(nNumFaces);
        for (auto face = 0; face < nNumFaces; ++face)
        {
            for (auto corner = 0; corner < 3; ++corner)
            {
                buildMesh.Faces[face].Corner[corner].Vertex = faceIndices[face * 3 + corner];
            }
        }

        target->build(buildBaseMesh, buildMesh);
        //target->optimizeMaterialUsage();
        return target;
    }
}
