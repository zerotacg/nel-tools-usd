module;

#include <nel/3d/texture.h>
#include <pxr/usd/usdHydra/tokens.h>

module nel_tools.usd.shape_to_usd.convert.material;

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
}
