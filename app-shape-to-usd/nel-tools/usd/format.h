#ifndef NEL_TOOLS_USD_FORMAT_H
#define NEL_TOOLS_USD_FORMAT_H

#include <string_view>
#include <fmt/format.h>
#include <nel/3d/texture.h>

template <> struct fmt::formatter<NL3D::ITexture::TUploadFormat>: formatter<std::string_view> {

    auto format(NL3D::ITexture::TUploadFormat source, format_context& ctx) const
      -> format_context::iterator;
};


#endif //NEL_TOOLS_USD_FORMAT_H
