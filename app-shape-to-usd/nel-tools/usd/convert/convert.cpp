#include <nel-tools/usd/convert/convert.h>

namespace nel_tools::usd::convert
{
    using namespace NL3D;
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
            target.emplace_back(uv->U, uv->V);
        }

        return target;
    }

}