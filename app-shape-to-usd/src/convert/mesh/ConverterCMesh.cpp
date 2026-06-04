module;
#include <string>
#include <ranges>
#include <fmt/color.h>
#include <fmt/format.h>
#include <nel/3d/material.h>
#include <nel/3d/mesh.h>
#include <nel/3d/texture.h>
#include <nel/misc/common.h>
#include <nel/misc/path.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/kind/registry.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/modelAPI.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdGeom/subset.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdHydra/tokens.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/shader.h>
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
        auto material = defineMaterial(0);
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
        auto material = defineMaterial(materialIndex);

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
            auto target = defineMaterial(i);
            convert(target, mesh->getMaterial(i));
        }
    }

    void ConverterCMesh::convert(UsdShadeMaterial& target, const CMaterial& source)
    {
        auto root = target.GetPath();
        auto pbrShader = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("PBRShader")));
        material::shader(pbrShader, source);
        auto diffuseColor = pbrShader.GetInput(common::UsdPreviewSurfaceTokens.inputs.diffuseColor);
        target.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);

        auto uvmap = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("uvmap")));
        uvmap.CreateIdAttr().Set(common::UsdPrimvarReader_float2Tokens.id);
        uvmap.CreateInput(common::UsdPrimvarReader_float2Tokens.inputs.varname, SdfValueTypeNames->String).Set(stPrimvarName);
        auto uvmapResult = uvmap.CreateOutput(common::UsdPrimvarReader_float2Tokens.outputs.result, SdfValueTypeNames->Float2);

        if (source.getDoubleSided())
        {
            nldebug("Material DoubleSided");
        }
        if (source.getBlend())
        {
            nldebug("Material Blend");
        }
        for (auto textureIndex = 0; textureIndex < IDRV_MAT_MAXTEXTURES; ++textureIndex)
        {
            if (source.texturePresent(textureIndex))
            {
                auto sourceTexture = source.getTexture(textureIndex);
                auto sampler = convert(root, sourceTexture, textureIndex);
                if (auto input = sampler.GetInput(common::UsdUVTextureTokens.inputs.st))
                {
                    input.ConnectToSource(uvmapResult);
                }
                diffuseColor.ConnectToSource(sampler.ConnectableAPI(), common::UsdUVTextureTokens.outputs.rgb);
                pbrShader.CreateInput(common::UsdPreviewSurfaceTokens.inputs.opacity, SdfValueTypeNames->Float).
                          ConnectToSource(
                              sampler.ConnectableAPI(), common::UsdUVTextureTokens.outputs.a);
            }
        }
    }

    UsdShadeShader ConverterCMesh::convert(SdfPath& root, ITexture* source, uint32 index)
    {
        auto shader = UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
        material::convert(settings, shader, source);
        return shader;
    }

    UsdShadeMaterial ConverterCMesh::defineMaterial(uint materialIndex)
    {
        return UsdShadeMaterial::Define(stage, Paths.material(materialIndex));
    }
}
