#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Eq;

class convert_face_indices_test : public testing::Test
{
protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(convert_face_indices_test, shouldStartAtMinBaseport)
{
	EXPECT_THAT(19, Eq(10));
}
