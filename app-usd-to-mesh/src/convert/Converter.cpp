module;
#include <fmt/color.h>
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
#include <pxr/usd/usdGeom/primvarsAPI.h>

module nel_tools.usd.usd_to_mesh.convert.Converter;
import nel_tools.usd.format;
import nel_tools.usd.usd_to_mesh.convert;


namespace nel_tools::usd::usd_to_mesh::convert
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;
    using namespace pxr;

    UsdGeomMesh findMesh(const auto& stage)
    {
        auto prim = stage->GetDefaultPrim();
        auto isMesh = [](const UsdPrim& p) { return p.IsA<UsdGeomMesh>(); };
        for (auto child : stage->Traverse() | std::views::filter(isMesh))
        {
            return UsdGeomMesh(child);
        }
        return UsdGeomMesh(prim);
    }

    bool isTriangular(const auto& mesh)
    {
        VtArray<int> faceCounts;

        mesh.GetFaceVertexCountsAttr().Get(&faceCounts);

        return ranges::all_of(faceCounts, [](int count) { return count == 3; });
    }

    CMaterial defaultMaterial()
    {
        CMaterial material;
        material.initLighted();
        material.setLighting(true, CRGBA::Black, CRGBA::White, CRGBA::White, CRGBA::Black);

        return material;
    }

    vector<CMaterial> buildMaterials(const auto& stage)
    {
        vector<CMaterial> materials;
        materials.push_back(defaultMaterial());

        return materials;
    }

    bool hasPerFaceVertexNormals(const UsdGeomMesh& mesh)
    {
        return mesh.GetNormalsInterpolation() == UsdGeomTokens->faceVarying;
    }

    auto Converter::run() -> unique_ptr<IShape>
    {
        UsdGeomMesh mesh = findMesh(stage);
        if (!mesh)
        {
            throw std::invalid_argument("Failed to find mesh prim");
        }

        if (!isTriangular(mesh))
        {
            throw std::invalid_argument("Mesh is not triangular, all face counts need to be 3");
        }

        auto target = make_unique<CMesh>();
        CMeshBase::CMeshBaseBuild buildBaseMesh;
        // todo implement mesh base build
        // @see CExportNel::buildBaseMeshInterface (buildBaseMesh, maxBaseBuild, node, time, nodeMatrix);
        // @see CExportNel::buildMaterials
        buildBaseMesh.Materials = buildMaterials(stage);

        // todo implement mesh build @see CExportNel::buildMeshInterface (*tri, buildMesh, buildBaseMesh, maxBaseBuild, node, time, nodeMap);
        CMesh::CMeshBuild buildMesh;
        buildMesh.Vertices = vertices(mesh.GetPointsAttr());
        if (buildMesh.Vertices.empty())
        {
            throw std::invalid_argument("Mesh has no points");
        }
        buildMesh.VertexFlags = CVertexBuffer::PositionFlag;

        VtArray<int> faceIndices = indices(mesh.GetFaceVertexIndicesAttr());
        auto nNumFaces = mesh.GetFaceCount();
        buildMesh.Faces.resize(nNumFaces);
        for (auto face = 0; face < nNumFaces; ++face)
        {
            for (auto corner = 0; corner < 3; ++corner)
            {
                buildMesh.Faces[face].Corner[corner].Vertex = faceIndices[(face * 3 + corner)];
            }
        }
        auto normals = vertices(mesh.GetNormalsAttr());
        if (!normals.empty())
        {
            buildMesh.VertexFlags |= CVertexBuffer::NormalFlag;
            for (auto face = 0; face < nNumFaces; ++face)
            {
                for (auto corner = 0; corner < 3; ++corner)
                {
                    buildMesh.Faces[face].Corner[corner].Normal = normals[(face * 3 + corner)];
                }
            }
        }
        if (hasPerFaceVertexNormals(mesh))
        {
            std::vector<CVector> vertices;
            vertices.reserve(nNumFaces * 3);
            for (auto face = 0; face < nNumFaces; ++face)
            {
                for (auto corner = 0; corner < 3; ++corner)
                {
                    auto index = face * 3 + corner;
                    buildMesh.Faces[face].Corner[corner].Vertex = index;
                    vertices.push_back(buildMesh.Vertices[faceIndices[index]]);
                }
            }
            buildMesh.Vertices = vertices;
        }
        auto primVarSt = UsdGeomPrimvarsAPI(mesh).GetPrimvar(Tokens.st);
        auto uvs = uv(primVarSt);
        if (!uvs.empty())
        {
            buildMesh.VertexFlags |= CVertexBuffer::TexCoord0Flag;
            for (auto face = 0; face < nNumFaces; ++face)
            {
                for (auto corner = 0; corner < 3; ++corner)
                {
                    auto uv = uvs[face * 3 + corner];
                    buildMesh.Faces[face].Corner[corner].Uvws[0].U = uv.U;
                    buildMesh.Faces[face].Corner[corner].Uvws[0].V = uv.V;
                    buildMesh.Faces[face].Corner[corner].Uvws[0].W = 0;
                }
            }
        }

        target->build(buildBaseMesh, buildMesh);
        //target->optimizeMaterialUsage();
        return target;
    }
}
