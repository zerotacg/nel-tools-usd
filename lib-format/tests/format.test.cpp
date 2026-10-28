#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>
#include <nel/3d/texture.h>

import nel_tools.usd.format;

using testing::StrEq;
using namespace NL3D;

struct FormatCase {
	ITexture::TUploadFormat value;
	std::string expected;
};

class format_test : public testing::TestWithParam<FormatCase>
{
};

TEST_P(format_test, should_format)
{
	FormatCase param = GetParam();
	EXPECT_THAT(fmt::format("{}", param.value), StrEq(param.expected));
}

INSTANTIATE_TEST_SUITE_P(
	TUploadFormat,
	format_test,
	testing::ValuesIn({
		FormatCase{ITexture::Auto, "Auto"},
		FormatCase{ITexture::RGBA8888, "RGBA8888"},
		FormatCase{ITexture::RGBA4444, "RGBA4444"},
		FormatCase{ITexture::RGBA5551, "RGBA5551"},
		FormatCase{ITexture::RGB888, "RGB888"},
		FormatCase{ITexture::RGB565, "RGB565"},
		FormatCase{ITexture::DXTC1, "DXTC1"},
		FormatCase{ITexture::DXTC1Alpha, "DXTC1Alpha"},
		FormatCase{ITexture::DXTC3, "DXTC3"},
		FormatCase{ITexture::DXTC5, "DXTC5"},
		FormatCase{ITexture::Luminance, "Luminance"},
		FormatCase{ITexture::Alpha, "Alpha"},
		FormatCase{ITexture::AlphaLuminance, "AlphaLuminance"},
		FormatCase{ITexture::DsDt, "DsDt"},
		FormatCase{ITexture::UploadFormatCount, "UploadFormatCount"},
		FormatCase{static_cast<ITexture::TUploadFormat>(-1), "unknown"},
		FormatCase{static_cast<ITexture::TUploadFormat>(999), "unknown"}
	})
);
