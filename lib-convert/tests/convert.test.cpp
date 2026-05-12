#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nel/3d/vertex_buffer.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/vt/array.h>

import nel_tools.usd.shape_to_usd.convert;

using testing::Eq;
using namespace NL3D;
using namespace pxr;
using namespace nel_tools::usd::shape_to_usd;

class convert_test : public testing::Test
{
protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(convert_test, should_convert_vertices)
{
	CVertexBuffer buffer;
	buffer.setVertexFormat(CVertexBuffer::PositionFlag);
	buffer.setNumVertices(3);
	CVertexBufferReadWrite io;
	buffer.lock(io);
	io.setVertexCoord(0, 0.1f, 0.2f, 0.3f);
	io.setVertexCoord(1, 0.4f, 0.5f, 0.6f);
	io.setVertexCoord(2, 0.7f, 0.8f, 0.9f);
	io.unlock();

	EXPECT_THAT(convert::vertices(buffer), Eq(VtArray<GfVec3f>{{0.1f, 0.2f, 0.3f}, {0.4f, 0.5f, 0.6f}, {0.7f, 0.8f, 0.9f}}));
}
