#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nel/3d/vertex_buffer.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/vt/array.h>

import nel_tools.usd.shape_to_usd.convert.material;
import nel_tools.usd.shape_to_usd.convert.material.TextureSettings;

using testing::Eq;
using testing::StrEq;
using namespace NL3D;
using namespace pxr;
using namespace nel_tools::usd::shape_to_usd;

class transform_filename_test : public testing::Test
{
protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(transform_filename_test, should_return_filename_as_is)
{
	convert::material::TextureSettings settings = {
		.convertToLowerCase = false,
		.extension = std::nullopt,
		.prefix = std::nullopt
	};

	EXPECT_THAT(convert::material::transformFilename(settings, "FILE-name"), StrEq("FILE-name"));
}

TEST_F(transform_filename_test, should_convert_filename_to_lowercase)
{
	convert::material::TextureSettings settings = {
		.convertToLowerCase = true,
		.extension = std::nullopt,
		.prefix = std::nullopt
	};

	EXPECT_THAT(convert::material::transformFilename(settings, "FILE-name"), StrEq("file-name"));
}

TEST_F(transform_filename_test, should_prefix_filename)
{
	convert::material::TextureSettings settings = {
		.convertToLowerCase = false,
		.extension = std::nullopt,
		.prefix = "prefix_"
	};

	EXPECT_THAT(convert::material::transformFilename(settings, "FILE-name"), StrEq("prefix_FILE-name"));
}

TEST_F(transform_filename_test, should_replace_extension)
{
	convert::material::TextureSettings settings = {
		.convertToLowerCase = false,
		.extension ="ext",
		.prefix = std::nullopt,
	};

	EXPECT_THAT(convert::material::transformFilename(settings, "FILE-name.with.extension"), StrEq("FILE-name.with.ext"));
}
