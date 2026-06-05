module;
#include <string>
#include <ranges>
#include <fmt/format.h>
#include <nel/3d/mesh_mrm_skinned.h>
#include <nel/misc/common.h>
#include <nel/misc/path.h>
#include <pxr/usd/kind/registry.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/modelAPI.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdGeom/subset.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdHydra/tokens.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/tokens.h>

module nel_tools.usd.shape_to_usd.convert.mesh.ConverterCMeshMRMSkinned;
import nel_tools.usd.common;
import nel_tools.usd.format;
import nel_tools.usd.shape_to_usd.convert;
import nel_tools.usd.shape_to_usd.convert.material;
import nel_tools.usd.shape_to_usd.paths;
import nel_tools.usd.shape_to_usd.tokens;


namespace nel_tools::usd::shape_to_usd::convert::mesh
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;
    using namespace pxr;
    using namespace paths;
    using namespace tokens;

    bool hasDoubleSidedMaterials(const CMeshMRMSkinned* mesh)
    {
        return ranges::any_of(
            views::iota(0u, mesh->getNbMaterial()),
            [mesh](auto index) -> bool { return mesh->getMaterial(index).getDoubleSided(); }
        );
    }

    void ConverterCMeshMRMSkinned::run()
    {
        modelRoot.GetPrim().GetVariantSets().AddVariantSet("textureSet");
        UsdModelAPI(modelRoot).SetKind(KindTokens->component);
        auto modelRootXform = UsdGeomXformCommonAPI(modelRoot);
        value(modelRootXform, *mesh);

        auto outMesh = UsdGeomMesh::Define(stage, Paths.model);

        outMesh.CreateDoubleSidedAttr().Set(hasDoubleSidedMaterials(mesh));
        outMesh.CreateSubdivisionSchemeAttr().Set(UsdGeomTokens->none);
        CVertexBuffer vertexBuffer;
        mesh->getVertexBuffer(vertexBuffer);
        value(outMesh, vertexBuffer);


        outMesh.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
        auto material = material::define(stage, 0);
        auto materialBindingAPI = UsdShadeMaterialBindingAPI(outMesh);
        materialBindingAPI.Bind(material);
        materialBindingAPI.SetMaterialBindSubsetsFamilyType(UsdGeomTokens->nonOverlapping);


        materials();
        subsets();

        auto indices = convertIndices();
        outMesh.CreateFaceVertexIndicesAttr().Set(indices);
        outMesh.CreateFaceVertexCountsAttr().Set(convertFaceCount(indices));
    }

    VtArray<int> ConverterCMeshMRMSkinned::convertIndices() const
    {
        const auto& meshIn = mesh->getMeshGeom();
        const auto& geomorphs = meshIn.getGeomorphs(lodId);
        VtArray<int> all;
        for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
        {
            CIndexBuffer indexBuffer;
            mesh->getRdrPassPrimitiveBlock(lodId, renderPass, indexBuffer);

            auto pass = value(indexBuffer, geomorphs);
            all.insert(all.end(), pass.begin(), pass.end());
        }

        return all;
    }

    VtArray<int> ConverterCMeshMRMSkinned::convertFaceCount(const VtArray<int>& source) const
    {
        return VtArray(source.size() / 3, 3);
    }

    void ConverterCMeshMRMSkinned::subsets()
    {
        if (mesh->getNbMaterial() <= 1)
        {
            return;
        }

        int faceCount = 0;
        for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
        {
            faceCount += subset(renderPass, faceCount);
        }
    }

    size_t ConverterCMeshMRMSkinned::subset(uint renderPass, int faceOffset)
    {
        auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
        CIndexBuffer indexBuffer;
        mesh->getRdrPassPrimitiveBlock(lodId, renderPass, indexBuffer);
        auto material = material::define(stage, materialIndex);

        auto subset = UsdGeomSubset::Define(stage, paths::subset(renderPass));

        subset.CreateElementTypeAttr().Set(UsdGeomTokens->face);
        subset.CreateFamilyNameAttr().Set(UsdShadeTokens->materialBind);
        auto subsetFaceIndices = faceIndices(indexBuffer, faceOffset);
        subset.CreateIndicesAttr().Set(subsetFaceIndices);
        subset.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
        UsdShadeMaterialBindingAPI(subset).Bind(material);

        return subsetFaceIndices.size();
    }

    void ConverterCMeshMRMSkinned::materials()
    {
        UsdGeomScope::Define(stage, Paths.materials);
        for (auto i = 0; i < mesh->getNbMaterial(); ++i)
        {
            auto target = material::define(stage, i);
            material::convert(settings, target, mesh->getMaterial(i));
        }
    }
}
