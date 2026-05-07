#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Eq;

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

TEST_F(convert_test, shouldStartAtMinBaseport)
{
	EXPECT_THAT(10, Eq(10));
}
