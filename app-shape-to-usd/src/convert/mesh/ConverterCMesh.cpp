module;
#include <string>
#include <ranges>
#include <fmt/format.h>
#include <nel/3d/mesh.h>
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

module nel_tools.usd.shape_to_usd.convert.mesh.ConverterCMesh;
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

    bool hasDoubleSidedMaterials(const CMesh* mesh)
    {
        return ranges::any_of(
            views::iota(0u, mesh->getNbMaterial()),
            [mesh](auto index) -> bool { return mesh->getMaterial(index).getDoubleSided(); }
        );
    }

    void ConverterCMesh::run()
    {
        modelRoot.GetPrim().GetVariantSets().AddVariantSet("textureSet");
        UsdModelAPI(modelRoot).SetKind(KindTokens->component);
        auto modelRootXform = UsdGeomXformCommonAPI(modelRoot);
        value(modelRootXform, *mesh);

        auto outMesh = UsdGeomMesh::Define(stage, Paths.model);

        outMesh.CreateDoubleSidedAttr().Set(hasDoubleSidedMaterials(mesh));
        outMesh.CreateSubdivisionSchemeAttr().Set(UsdGeomTokens->none);
        value(outMesh, mesh->getVertexBuffer());


        outMesh.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
        auto material = material::define(stage, 0);
        auto materialBindingAPI = UsdShadeMaterialBindingAPI(outMesh);
        materialBindingAPI.Bind(material);
        materialBindingAPI.SetMaterialBindSubsetsFamilyType(UsdGeomTokens->nonOverlapping);


        materials();
        subsets(Paths.model);

        auto indices = convertIndices();
        outMesh.CreateFaceVertexIndicesAttr().Set(indices);
        outMesh.CreateFaceVertexCountsAttr().Set(convertFaceCount(indices));
    }

    VtArray<int> ConverterCMesh::convertIndices() const
    {
        VtArray<int> all;
        for (auto renderPass = 0; mesh->getNbMatrixBlock() > lodId && renderPass < mesh->getNbRdrPass(lodId); ++
             renderPass)
        {
            auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);

            auto pass = value(indexBuffer);
            all.insert(all.end(), pass.begin(), pass.end());
        }

        return all;
    }

    VtArray<int> ConverterCMesh::convertFaceCount(VtArray<int>& source) const
    {
        VtArray<int> target;

        for (auto i = 0; i < source.size(); i += 3)
        {
            target.emplace_back(3);
        }

        return target;
    }

    VtArray<int> ConverterCMesh::convertFaceIndices(const CIndexBuffer& source, int offset) const
    {
        VtArray<int> target;
        CIndexBufferRead indexBufferRead;
        source.lock(indexBufferRead);

        for (auto i = 0; i < source.getNumIndexes() / 3; ++i)
        {
            // TODO handle -1 indices
            target.emplace_back(offset + i);
        }

        return target;
    }

    void ConverterCMesh::subsets(const SdfPath& root)
    {
        if (mesh->getNbMaterial() <= 1)
        {
            return;
        }

        int faceCount = 0;
        for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
        {
            faceCount += subset(root, renderPass, faceCount);
        }
    }

    size_t ConverterCMesh::subset(const SdfPath& root, uint renderPass, int faceOffset)
    {
        auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
        auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
        auto material = material::define(stage, materialIndex);

        auto subset = UsdGeomSubset::Define(stage, root.AppendPath(SdfPath(fmt::format("subset_{}", renderPass))));

        subset.CreateElementTypeAttr().Set(UsdGeomTokens->face);
        subset.CreateFamilyNameAttr().Set(UsdShadeTokens->materialBind);
        auto faceIndices = convertFaceIndices(indexBuffer, faceOffset);
        subset.CreateIndicesAttr().Set(faceIndices);
        subset.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
        UsdShadeMaterialBindingAPI(subset).Bind(material);

        return faceIndices.size();
    }

    void ConverterCMesh::materials()
    {
        UsdGeomScope::Define(stage, Paths.materials);
        for (auto i = 0; i < mesh->getNbMaterial(); ++i)
        {
            auto target = material::define(stage, i);
            material::convert(settings, target, mesh->getMaterial(i));
        }
    }
}
