module;
#include <nel/3d/index_buffer.h>
#include <nel/3d/texture.h>
#include <nel/3d/vertex_buffer.h>
#include <nel/misc/rgba.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/vt/array.h>
#include <pxr/usd/usdHydra/tokens.h>

module nel_tools.usd.convert;

namespace nel_tools::usd::convert
{
    using namespace NL3D;
    using namespace NLMISC;
    using namespace pxr;

    VtArray<GfVec3f> vertices(const CVertexBuffer& source)
    {
        VtArray<GfVec3f> target;
        CVertexBufferRead reader;
        source.lock(reader);

        for (auto i = 0; i < source.getNumVertices(); ++i)
        {
            const auto vertex = reader.getVertexCoordPointer(i);
            target.emplace_back(vertex->x, vertex->y, vertex->z);
        }

        return target;
    }

    VtArray<GfVec3f> normals(const CVertexBuffer& source)
    {
        VtArray<GfVec3f> target;
        CVertexBufferRead reader;
        source.lock(reader);

        for (auto i = 0; i < source.getNumVertices(); ++i)
        {
            const auto normal = reader.getNormalCoordPointer(i);
            target.emplace_back(normal->x, normal->y, normal->z);
        }

        return target;
    }

    VtArray<GfVec2f> uvs(const CVertexBuffer& source)
    {
        VtArray<GfVec2f> target;
        CVertexBufferRead reader;
        source.lock(reader);

        for (auto i = 0; i < source.getNumVertices(); ++i)
        {
            const auto uv = reader.getTexCoordPointer(i);
            target.emplace_back(uv->U, -uv->V);
        }

        return target;
    }

    VtArray<int> value(const CIndexBuffer& source)
    {
        VtArray<int> target;
        CIndexBufferRead reader;
        source.lock(reader);

        for (auto i = 0; i < source.getNumIndexes(); ++i)
        {
            if (auto index = getIndexAt(reader, i); index != -1)
            {
                target.emplace_back(index);
            }
        }

        return target;
    }

    int getIndexAt(const CIndexBufferRead& reader, const int index)
    {
        switch (reader.getFormat())
        {
        case CIndexBuffer::Indices32:
            return *(static_cast<const uint32*>(reader.getPtr()) + index);
        case CIndexBuffer::Indices16:
        default:
            return *(static_cast<const uint16*>(reader.getPtr()) + index);
        }
    }

    GfVec4f value(const CRGBAF& source)
    {
        return {source.R, source.G, source.B, source.A};
    }

    GfVec3f rgb(const CRGBAF& source)
    {
        return {source.R, source.G, source.B};
    }


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
