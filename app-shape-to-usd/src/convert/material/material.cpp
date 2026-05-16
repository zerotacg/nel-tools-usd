module;

#include <fmt/format.h>
#include <nel/3d/texture.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>
#include <nel/misc/path.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/editContext.h>
#include <pxr/usd/usd/variantSets.h>
#include <pxr/usd/usdHydra/tokens.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdShade/shader.h>

module nel_tools.usd.shape_to_usd.convert.material;
import nel_tools.usd.common;
import nel_tools.usd.shape_to_usd.convert.material.TextureSettings;

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
}
