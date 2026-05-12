module;
#include <string>
#include <ranges>
#include <fmt/color.h>
#include <fmt/format.h>
#include <nel/3d/material.h>
#include <nel/3d/mesh.h>
#include <nel/3d/texture.h>
#include <nel/3d/texture_cube.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>
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
#include <pxr/usd/usdHydra/tokens.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdShade/tokens.h>

module nel_tools.usd.shape_to_usd.convert.mesh.ConverterCMesh;
import nel_tools.usd.format;
import nel_tools.usd.shape_to_usd.convert;


namespace nel_tools::usd::shape_to_usd::convert::mesh
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;
    using namespace pxr;

    bool hasDoubleSidedMaterials(const CMesh* mesh)
    {
        return ranges::any_of(
            views::iota(0u, mesh->getNbMaterial()),
            [mesh](auto index) -> bool { return mesh->getMaterial(index).getDoubleSided(); }
        );
    }

    void ConverterCMesh::run()
    {
        UsdGeomSetStageUpAxis(stage, UsdGeomTokens->z);
        UsdGeomSetStageMetersPerUnit(stage, 1.0);
        stage->GetRootLayer()->SetDefaultPrim(Tokens.root);
        auto modelRoot = UsdGeomXform::Define(stage, Paths.root);
        UsdModelAPI(modelRoot).SetKind(KindTokens->component);

        auto outMesh = UsdGeomMesh::Define(stage, Paths.model);

        outMesh.CreateDoubleSidedAttr().Set(hasDoubleSidedMaterials(mesh));
        outMesh.CreateSubdivisionSchemeAttr().Set(UsdGeomTokens->none);
        outMesh.CreatePointsAttr().Set(vertices(mesh->getVertexBuffer()));
        outMesh.CreateNormalsAttr().Set(normals(mesh->getVertexBuffer()));
        outMesh.SetNormalsInterpolation(UsdGeomTokens->vertex);

        if (mesh->getVertexBuffer().getVertexFormat() & CVertexBuffer::TexCoord0Flag)
        {
            UsdGeomPrimvarsAPI(outMesh)
                .CreatePrimvar(Tokens.st, SdfValueTypeNames->TexCoord2fArray, UsdGeomTokens->vertex)
                .Set(uvs(mesh->getVertexBuffer()));
        }


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
        for (auto renderPass = 0; mesh->getNbMatrixBlock() > lodId && renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
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
        pbrShader.CreateIdAttr().Set(UsdPreviewSurfaceTokens.id);
        auto diffuseColor = pbrShader.CreateInput(UsdPreviewSurfaceTokens.inputs.diffuseColor,
                                                  SdfValueTypeNames->Color3f);
        diffuseColor.Set(rgb(source.getDiffuse()));
        pbrShader.CreateInput(UsdPreviewSurfaceTokens.inputs.useSpecularWorkflow, SdfValueTypeNames->Int).Set(1);
        pbrShader.CreateInput(UsdPreviewSurfaceTokens.inputs.specularColor, SdfValueTypeNames->Color3f).Set(
            rgb(source.getSpecular()));
        pbrShader.CreateInput(UsdPreviewSurfaceTokens.inputs.roughness, SdfValueTypeNames->Float).Set(0.9f);
        if (source.getAlphaTest())
        {
            pbrShader.CreateInput(UsdPreviewSurfaceTokens.inputs.opacityThreshold, SdfValueTypeNames->Float).Set(
                source.getAlphaTestThreshold());
        }
        target.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);

        auto uvmap = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("uvmap")));
        uvmap.CreateIdAttr().Set(TfToken("UsdPrimvarReader_float2"));
        uvmap.CreateInput(Tokens.varname, SdfValueTypeNames->String).Set(stPrimvarName);
        auto uvmapResult = uvmap.CreateOutput(Tokens.result, SdfValueTypeNames->Float2);

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
                if (auto input = sampler.GetInput(Tokens.st))
                {
                    input.ConnectToSource(uvmapResult);
                }
                diffuseColor.ConnectToSource(sampler.ConnectableAPI(), UsdUVTextureTokens.outputs.rgb);
                pbrShader.CreateInput(UsdPreviewSurfaceTokens.inputs.opacity, SdfValueTypeNames->Float).ConnectToSource(
                    sampler.ConnectableAPI(), UsdUVTextureTokens.outputs.a);
            }
        }
    }

    UsdShadeShader ConverterCMesh::convert(SdfPath& root, ITexture* source, uint32 index)
    {
        fmt::print(fg(fmt::terminal_color::blue), "Texture at index {} is {} upload format {}\n", index,
                   source->getClassName(), source->getUploadFormat());

        if (const auto specific = dynamic_cast<CTextureFile*>(source))
        {
            return convert(root, *specific, index);
        }
        else if (const auto specific = dynamic_cast<CTextureMultiFile*>(source))
        {
            nldebug("CTextureMultiFile count %i", specific->getNumFileName());
            for (auto i = 0; i < specific->getNumFileName(); ++i)
            {
                const auto& fileName = specific->getFileName(i);
                nldebug("CTextureMultiFile %i %s ", i, fileName.c_str());
            }
        }
        else if (const auto specific = dynamic_cast<CTextureCube*>(source))
        {
            nldebug("CTextureCube");
        }
        else
        {
            nlwarning("Texture type not supported", source->getClassName().c_str());
        }

        return UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
    }

    UsdShadeShader ConverterCMesh::convert(const SdfPath& root, const CTextureFile& source, uint32 index) const
    {
        nldebug("CTextureFile %s", source.getFileName().c_str());
        auto fileName = transformFilename(source.getFileName());
        auto sampler = UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
        sampler.CreateIdAttr().Set(UsdUVTextureTokens.id);
        sampler.CreateInput(UsdUVTextureTokens.inputs.file, SdfValueTypeNames->Asset).Set(SdfAssetPath(fileName));
        sampler.CreateInput(UsdUVTextureTokens.inputs.st, SdfValueTypeNames->Float2);
        sampler.CreateInput(UsdUVTextureTokens.inputs.wrapS, SdfValueTypeNames->Token).Set(
            value(source.getWrapS()));
        sampler.CreateInput(UsdUVTextureTokens.inputs.wrapT, SdfValueTypeNames->Token).Set(
            value(source.getWrapT()));
        sampler.CreateInput(UsdHydraTokens->magFilter, SdfValueTypeNames->Token).Set(
            value(source.getMagFilter()));
        sampler.CreateInput(UsdHydraTokens->minFilter, SdfValueTypeNames->Token).Set(
            value(source.getMinFilter()));
        sampler.CreateOutput(UsdUVTextureTokens.outputs.rgb, SdfValueTypeNames->Float3);

        return sampler;
    }

    UsdShadeMaterial ConverterCMesh::defineMaterial(uint materialIndex)
    {
        return UsdShadeMaterial::Define(
            stage, Paths.materials.AppendPath(SdfPath(fmt::format("material_{}_MAT", materialIndex))));
    }

    string ConverterCMesh::transformFilename(const string& input) const
    {
        auto transformed = input;

        if (settings.convertToLowerCase)
        {
            transformed = toLower(transformed);
        }

        if (settings.replaceExtension)
        {
            transformed = CFile::getFilenameWithoutExtension(transformed);
            transformed += ".";
            transformed += "png";
        }

        return settings.prefix + transformed;
    }
}
