module;

#include <nel/3d/material.h>
#include <nel/3d/texture_file.h>
#include <nel/misc/path.h>
#include <pxr/usd/usdShade/material.h>

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

    CMaterial convert(const TextureSettings& settings, const UsdShadeMaterial& source)
    {
        auto surface = source.ComputeSurfaceSource();
        if (!surface)
        {
            return defaultMaterial();
        }

        CMaterial target = defaultMaterial();
        if (auto diffuseColor = surface.GetInput(TfToken("diffuseColor")))
        {
            GfVec3f value;
            if (diffuseColor.Get(&value))
            {
                target.setDiffuse(rgb(value));
            }
            if (diffuseColor.HasConnectedSource())
            {
                auto sampler = UsdShadeShader(diffuseColor.GetConnectedSources().front().source);
                TfToken id;
                sampler.GetIdAttr().Get(&id);
                if (id == common::UsdUVTextureTokens.id)
                {
                    SdfAssetPath value;
                    sampler.GetInput(TfToken("file")).Get(&value);
                    auto filename = value.GetAssetPath();
                    if (settings.removeTextureFilePath)
                    {
                        filename = CFile::getFilename(filename);
                    }
                    auto* texture = new CTextureFile(filename);
                    target.setTexture(0, texture);
                }
            }
        }

        return target;
    }
}
