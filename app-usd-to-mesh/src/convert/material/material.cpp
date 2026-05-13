module;

#include <memory>
#include <ranges>
#include <fmt/format.h>
#include <nel/3d/material.h>
#include <nel/3d/texture_file.h>
#include <nel/misc/path.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/shader.h>

module nel_tools.usd.usd_to_mesh.convert.material;
import nel_tools.usd.common;
import nel_tools.usd.usd_to_mesh.convert;
import nel_tools.usd.usd_to_mesh.convert.material.TextureSettings;

namespace nel_tools::usd::usd_to_mesh::convert::material
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace std;
    using namespace pxr;

    CMaterial defaultMaterial()
    {
        CMaterial material;
        material.initLighted();
        material.setLighting(true, CRGBA::Black, CRGBA::White, CRGBA::White, CRGBA::Black);

        return material;
    }

    auto isTexture(const UsdPrim& prim)
    {
        TfToken id;
        if (auto shader = UsdShadeShader(prim))
        {
            shader.GetIdAttr().Get(&id);
        }

        return id == common::UsdUVTextureTokens.id;
    };

    vector<UsdShadeShader> findTextures(const UsdShadeMaterial& material)
    {
        vector<UsdShadeShader> target;

        for (auto child : material.GetPrim().GetChildren() | std::views::filter(isTexture))
        {
            target.emplace_back(child);
        }

        return target;
    }

    CMaterial convert(const TextureSettings& settings, const UsdShadeMaterial& source)
    {
        CMaterial target = defaultMaterial();
        auto surface = source.ComputeSurfaceSource();
        if (!surface)
        {
            return target;
        }

        if (auto diffuseColor = surface.GetInput(TfToken("diffuseColor")))
        {
            GfVec3f value;
            if (diffuseColor.Get(&value))
            {
                target.setDiffuse(rgb(value));
            }
        }

        auto textures = findTextures(source);

        for (auto i = 0; i < std::min<uint32>(textures.size(), IDRV_MAT_MAXTEXTURES); ++i)
        {
            unique_ptr<ITexture> texture = convert(settings, textures[i]);
            target.setTexture(i, texture.release());
        }

        return target;
    }

    unique_ptr<ITexture> convert(const TextureSettings& settings, const UsdShadeShader& source)
    {
        TfToken id;
        source.GetIdAttr().Get(&id);
        if (id != common::UsdUVTextureTokens.id)
        {
            throw std::invalid_argument(fmt::format("Shader is not a texture id: {}", id.GetString()));
        }
        SdfAssetPath value;
        source.GetInput(TfToken("file")).Get(&value);
        auto filename = value.GetAssetPath();
        if (settings.removeTextureFilePath)
        {
            filename = CFile::getFilename(filename);
        }
        return make_unique<CTextureFile>(filename);
    }
}
