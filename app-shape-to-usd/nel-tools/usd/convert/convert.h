#ifndef NEL_TOOLS_USD_CONVERT_H
#define NEL_TOOLS_USD_CONVERT_H

#include <nel/3d/vertex_buffer.h>
#include <nel/misc/rgba.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/vt/array.h>

namespace nel_tools::usd::convert
{
    pxr::VtArray<pxr::GfVec3f> vertices(const NL3D::CVertexBuffer& source);
    pxr::VtArray<pxr::GfVec3f> normals(const NL3D::CVertexBuffer& source);
    pxr::VtArray<pxr::GfVec2f> uvs(const NL3D::CVertexBuffer& source);
    pxr::GfVec4f value(const NLMISC::CRGBAF& source);
    pxr::GfVec3f rgb(const pxr::GfVec4f& source);
}

#endif //NEL_TOOLS_USD_CONVERT_H
