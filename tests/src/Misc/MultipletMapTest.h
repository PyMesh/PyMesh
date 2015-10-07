/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <vector>

#include <Core/Exception.h>
#include <Misc/Multiplet.h>
#include <Misc/MultipletMap.h>

class MultipletMapTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            for (size_t i=0; i<10; i++) {
                m_multiplets.push_back(Multiplet(i, i, i, i));
            }
        }

    protected:
        std::vector<Multiplet> m_multiplets;
};

TEST_F(MultipletMapTest, Empty) {
    MultipletMap<Float> empty;
    ASSERT_TRUE(empty.empty());
}

TEST_F(MultipletMapTest, UniqueKey) {
    MultipletMap<int> mmap;
    const size_t num_entries = m_multiplets.size();
    for (size_t i=0; i<num_entries; i++) {
        mmap.insert(m_multiplets[i], i);
    }

    ASSERT_EQ(num_entries, mmap.size());
    for (size_t i=0; i<num_entries; i++) {
        const auto& item = mmap.get(m_multiplets[i]);
        ASSERT_EQ(1, item.size());
        ASSERT_EQ(i, item[0]);
    }
}

TEST_F(MultipletMapTest, KeyCollision) {
    const size_t N=5;
    MultipletMap<int> mmap;
    auto key = m_multiplets[0];
    for (size_t i=0; i<N; i++) {
        mmap.insert(key, i);
    }

    ASSERT_EQ(1, mmap.size());
    const auto& items = mmap.get(key);
    ASSERT_EQ(N, items.size());

    // Since the order of insertion is not necessarily preserved.
    // I dump the item into a vector and sort it.
    std::vector<int> item_vec(items.begin(), items.end());
    std::sort(item_vec.begin(), item_vec.end());
    for (size_t i=0; i<N; i++) {
        ASSERT_EQ(i, item_vec[i]);
    }
}
