#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "stack.hpp"

using namespace stack;

TEST(AllAPITest, BadStackHandler)
{
    destroy(-1);
    EXPECT_EQ(valid(-1), false);
    EXPECT_EQ(count(-1), 0u);
    const int data_in = 1;
    push(-1, &data_in, sizeof(data_in));
    int data_out = 0;
    EXPECT_EQ(pop(-1, &data_out, sizeof(data_out)), 0u);
    EXPECT_EQ(data_out, 0);
}

TEST(AllocationTests, SingleAllocation)
{
    const Handle stack = create();
    EXPECT_EQ(valid(stack), true);
    EXPECT_EQ(count(stack), 0u);
    destroy(stack);
    EXPECT_EQ(valid(stack), false);
}

TEST(AllocationTests, SeveralAllocations)
{
    const size_t num = 10;
    Handle stacks[num] = {-1};
    for (size_t i = 0; i < num; ++i)
    {
        stacks[i] = create();
        EXPECT_EQ(valid(stacks[i]), true);
        EXPECT_EQ(count(stacks[i]), 0u);
    }
    for (size_t i = 0; i < num; ++i)
    {
        destroy(stacks[i]);
        EXPECT_EQ(valid(stacks[i]), false);
    }
}

struct ModifyTests : ::testing::Test
{
    void SetUp()
    {
        stack = create();
        ASSERT_EQ(valid(stack), true);
    }
    void TearDown()
    {
        destroy(stack);
        ASSERT_EQ(valid(stack), false);
    }
    Handle stack = -1;
};

TEST_F(ModifyTests, PushBadArgs)
{
    push(stack, nullptr, 0u);
    EXPECT_EQ(count(stack), 0u);

    const int data_out = 1;
    push(stack, &data_out, 0u);
    EXPECT_EQ(count(stack), 0u);

    push(stack, nullptr, sizeof(data_out));
    EXPECT_EQ(count(stack), 0u);
}

TEST_F(ModifyTests, PopBadArgs)
{
    const size_t size = 5;
    const int data_in[size] = {1};
    push(stack, &data_in, sizeof(data_in));
    ASSERT_EQ(count(stack), 1u);

    EXPECT_EQ(pop(stack, nullptr, 0u), 0u);

    int data_out[size - 1] = {0};
    EXPECT_EQ(pop(stack, data_out, sizeof(data_out)), 0u);
    EXPECT_THAT(data_out, ::testing::Each(0));

    EXPECT_EQ(pop(stack, nullptr, sizeof(data_in)), 0u);
    EXPECT_THAT(data_out, ::testing::Each(0));
}

TEST_F(ModifyTests, PopFromEmptyStack)
{
    ASSERT_EQ(count(stack), 0u);
    int data_out = 0;
    EXPECT_EQ(pop(stack, &data_out, sizeof(data_out)), 0u);
}

TEST_F(ModifyTests, SinglePushPop)
{
    const int data_in[3] = {0, 1, 2};
    int data_out[3] = {2, 1, 0};
    push(stack, data_in, sizeof(data_in));
    EXPECT_EQ(count(stack), 1u);
    EXPECT_EQ(pop(stack, data_out, sizeof(data_out)), sizeof(data_out));
    EXPECT_EQ(count(stack), 0u);
    EXPECT_THAT(data_out, ::testing::ElementsAre(0, 1, 2));
}

TEST_F(ModifyTests, SeveralPushPop)
{
    const size_t size = 3;
    const int data_in[size] = {0, 1, 2};
    int data_out[size] = {0, 1, 2};
    for (size_t i = 0; i < size; ++i)
    {
        push(stack, &data_in[i], sizeof(data_in[i]));
        EXPECT_EQ(count(stack), i + 1u);
    }
    for (size_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(pop(stack, &data_out[i], sizeof(data_out[i])),
            sizeof(data_out[i]));
        EXPECT_EQ(count(stack), size - 1u - i);
    }
    EXPECT_THAT(data_out, ::testing::ElementsAre(2, 1, 0));
}
