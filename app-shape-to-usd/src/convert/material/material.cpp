module;

#include <fmt/format.h>
#include <nel/3d/material.h>
#include <nel/3d/texture.h>
#include <nel/3d/texture_cube.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>
#include <nel/misc/path.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/editContext.h>
#include <pxr/usd/usd/variantSets.h>
#include <pxr/usd/usdHydra/tokens.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/shader.h>

module nel_tools.usd.shape_to_usd.convert.material;
import nel_tools.usd.common;
import nel_tools.usd.shape_to_usd.convert;
import nel_tools.usd.shape_to_usd.convert.material.TextureSettings;
import nel_tools.usd.shape_to_usd.paths;

namespace nel_tools::usd::shape_to_usd::convert::material
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;
    using namespace pxr;

    const TfToken& value(const ITexture::TWrapMode& source)
    {
        switch (source)
        {
        default:
        case ITexture::TWrapMode::Repeat:
            return UsdHydraTokens->repeat;
        case ITexture::TWrapMode::Clamp:
            return UsdHydraTokens->clamp;
        }
    }

    const TfToken& value(const ITexture::TMinFilter& source)
    {
        switch (source)
        {
        default:
        case ITexture::NearestMipMapOff:
            return UsdHydraTokens->nearest;
        case ITexture::NearestMipMapNearest:
            return UsdHydraTokens->nearestMipmapNearest;
        case ITexture::NearestMipMapLinear:
            return UsdHydraTokens->nearestMipmapLinear;
        case ITexture::LinearMipMapOff:
            return UsdHydraTokens->linear;
        case ITexture::LinearMipMapNearest:
            return UsdHydraTokens->linearMipmapNearest;
        case ITexture::LinearMipMapLinear:
            return UsdHydraTokens->linearMipmapLinear;
        }
    }

    const TfToken& value(const ITexture::TMagFilter& source)
    {
        switch (source)
        {
        default:
        case ITexture::Nearest:
            return UsdHydraTokens->nearest;
        case ITexture::Linear:
            return UsdHydraTokens->linear;
        }
    }

    void shader(UsdShadeShader& target, const CMaterial& source)
    {
        target.CreateIdAttr().Set(common::UsdPreviewSurfaceTokens.id);
        target.CreateInput(common::UsdPreviewSurfaceTokens.inputs.diffuseColor, SdfValueTypeNames->Color3f).Set(
            rgb(source.getDiffuse()));
        target.CreateInput(common::UsdPreviewSurfaceTokens.inputs.useSpecularWorkflow, SdfValueTypeNames->Int).
               Set(1);
        target.CreateInput(common::UsdPreviewSurfaceTokens.inputs.specularColor, SdfValueTypeNames->Color3f).Set(
            rgb(source.getSpecular()));
        target.CreateInput(common::UsdPreviewSurfaceTokens.inputs.roughness, SdfValueTypeNames->Float).Set(0.9f);
        if (source.getAlphaTest())
        {
            target.CreateInput(common::UsdPreviewSurfaceTokens.inputs.opacityThreshold, SdfValueTypeNames->Float).Set(
                source.getAlphaTestThreshold());
        }
    }

    void convert(const TextureSettings& settings, UsdShadeShader& target, ITexture* source )
    {
        if (const auto specific = dynamic_cast<const CTextureFile*>(source))
        {
            return convert(settings, target, *specific);
        }
        else if (const auto specific = dynamic_cast<const CTextureMultiFile*>(source))
        {
            return convert(settings, target, *specific);
        }
        else if (const auto specific = dynamic_cast<const CTextureCube*>(source))
        {
            nldebug("CTextureCube");
        }
        else
        {
            nlwarning("Texture type not supported", source->getClassName().c_str());
        }

        target.GetPrim().SetCustomDataByKey(TfToken("nel:class_name"), VtValue(source->getClassName()));

    }

    void convert(const TextureSettings& settings, UsdShadeShader& target, const CTextureFile& source)
    {
        nldebug("CTextureFile %s", source.getFileName().c_str());
        auto fileName = transformFilename(settings, source.getFileName());
        target.CreateIdAttr().Set(common::UsdUVTextureTokens.id);
        target.CreateInput(common::UsdUVTextureTokens.inputs.file, SdfValueTypeNames->Asset).Set(
            SdfAssetPath(fileName));
        target.CreateInput(common::UsdUVTextureTokens.inputs.st, SdfValueTypeNames->Float2);
        target.CreateInput(common::UsdUVTextureTokens.inputs.wrapS, SdfValueTypeNames->Token).Set(
            value(source.getWrapS()));
        target.CreateInput(common::UsdUVTextureTokens.inputs.wrapT, SdfValueTypeNames->Token).Set(
            value(source.getWrapT()));
        target.CreateInput(UsdHydraTokens->magFilter, SdfValueTypeNames->Token).Set(
            value(source.getMagFilter()));
        target.CreateInput(UsdHydraTokens->minFilter, SdfValueTypeNames->Token).Set(
            value(source.getMinFilter()));
        target.CreateOutput(common::UsdUVTextureTokens.outputs.rgb, SdfValueTypeNames->Float3);
    }

    void convert(const TextureSettings& settings, UsdShadeShader& target, const CTextureMultiFile& source)
    {
        auto stage = target.GetPrim().GetStage();
        auto modelRoot = stage->GetDefaultPrim();
        nldebug("CTextureMultiFile count %i", source.getNumFileName());
        auto modelVariants = modelRoot.GetVariantSet("textureSet");
        auto textureVariants = target.GetPrim().GetVariantSets().AddVariantSet("textureSet");
        target.CreateIdAttr().Set(common::UsdUVTextureTokens.id);
        auto inputFile = target.CreateInput(common::UsdUVTextureTokens.inputs.file, SdfValueTypeNames->Asset);
        target.CreateInput(common::UsdUVTextureTokens.inputs.st, SdfValueTypeNames->Float2);
        target.CreateInput(common::UsdUVTextureTokens.inputs.wrapS, SdfValueTypeNames->Token).Set(
            value(source.getWrapS()));
        target.CreateInput(common::UsdUVTextureTokens.inputs.wrapT, SdfValueTypeNames->Token).Set(
            value(source.getWrapT()));
        target.CreateInput(UsdHydraTokens->magFilter, SdfValueTypeNames->Token).Set(
            value(source.getMagFilter()));
        target.CreateInput(UsdHydraTokens->minFilter, SdfValueTypeNames->Token).Set(
            value(source.getMinFilter()));
        target.CreateOutput(common::UsdUVTextureTokens.outputs.rgb, SdfValueTypeNames->Float3);
        for (auto i = 0; i < source.getNumFileName(); ++i)
        {
            const auto& sourceFileName = source.getFileName(i);
            nldebug("CTextureMultiFile %i %s ", i, sourceFileName.c_str());
            auto fileName = transformFilename(settings, sourceFileName);
            auto variant = fmt::format("texture_{}", i);
            modelVariants.AddVariant(variant);
            modelVariants.SetVariantSelection(variant);
            textureVariants.AddVariant(variant);
            {
                auto ctxt = UsdEditContext(modelVariants.GetVariantEditContext());
                textureVariants.SetVariantSelection(variant);
            }
            {
                auto ctxt = UsdEditContext(textureVariants.GetVariantEditContext());
                inputFile.Set(SdfAssetPath(fileName));
            }
        }
        modelVariants.SetVariantSelection("texture_0");
    }

    string transformFilename(const TextureSettings& settings, const string& input)
    {
        auto transformed = input;

        if (settings.convertToLowerCase)
        {
            transformed = toLower(transformed);
        }

        if (auto extension = settings.extension)
        {
            transformed = CFile::getFilenameWithoutExtension(transformed);
            transformed += ".";
            transformed += *extension;
        }

        if (auto searchPath = settings.searchPath)
        {
            CPath::addSearchPath(*searchPath, true, false);
            auto found = CPath::lookup(transformed, false);
            if (!found.empty())
            {
                CPath::makePathRelative(*searchPath, found);
                transformed = found;
            }
        }

        if (auto prefix = settings.prefix)
        {
            transformed = *prefix + transformed;
        }

        return transformed;
    }

    void convert(const TextureSettings& settings, UsdShadeMaterial& target, const CMaterial& source)
    {
        auto root = target.GetPath();
        auto stage = target.GetPrim().GetStage();
        auto pbrShader = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("PBRShader")));
        shader(pbrShader, source);
        auto diffuseColor = pbrShader.GetInput(common::UsdPreviewSurfaceTokens.inputs.diffuseColor);
        target.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);

        auto uvmap = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("uvmap")));
        uvmap.CreateIdAttr().Set(common::UsdPrimvarReader_float2Tokens.id);
        uvmap.CreateInput(common::UsdPrimvarReader_float2Tokens.inputs.varname, SdfValueTypeNames->String).Set(common::stPrimvarName);
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
                auto sampler = UsdShadeShader::Define(stage, paths::texture(root, textureIndex));
                convert(settings, sampler, sourceTexture);
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

    UsdShadeMaterial define(UsdStageRefPtr& stage, uint index)
    {
        return UsdShadeMaterial::Define(stage, paths::material(index));
    }
}
