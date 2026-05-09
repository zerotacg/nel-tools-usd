module;
#include <nel/3d/vertex_buffer.h>
#include <nel/misc/rgba.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/vt/array.h>

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
            target.emplace_back(uv->U, - uv->V);
        }

        return target;
    }

    GfVec4f value(const CRGBAF& source)
    {
        return {source.R, source.G, source.B, source.A};
    }

    GfVec3f rgb(const CRGBAF& source)
    {
        return {source.R, source.G, source.B};
    }
}
