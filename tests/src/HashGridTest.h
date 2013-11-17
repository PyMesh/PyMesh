#pragma once
#include <HashGrid.h>

class HashGridTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            m_grid = Zhou::HashGrid::create(0.5);
        }

    protected:
        Zhou::HashGrid::Ptr m_grid;
};

TEST_F(HashGridTest, Empty) {
    ASSERT_EQ(0, m_grid->size());
}

TEST_F(HashGridTest, Empty2) {
    //Vector3F origin = Vector3F::Zero();
    //const Zhou::HashGrid::HashItem* items = m_grid->get_items(origin);
    //ASSERT_TRUE(items->empty());
}
