/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <vector>

#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>

class TripletMapTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            for (size_t i=0; i<10; i++) {
                m_triplets.push_back(Triplet(i, i, i));
            }
        }

    protected:
        std::vector<Triplet> m_triplets;
};

TEST_F(TripletMapTest, Empty) {
    TripletMap<Float> t_map;
    ASSERT_TRUE(t_map.empty());
    ASSERT_EQ(0, t_map.size());
    ASSERT_TRUE(t_map.begin() == t_map.end());
    ASSERT_THROW(t_map.get(m_triplets[0]), RuntimeError);
}

TEST_F(TripletMapTest, UniqueKey) {
    TripletMap<int> t_map;
    const size_t num_triplets = m_triplets.size();
    for (size_t i=0; i<num_triplets; i++) {
        t_map.insert(m_triplets[i], i);
    }

    ASSERT_EQ(num_triplets, t_map.size());
    for (size_t i=0; i<num_triplets; i++) {
        const TripletMap<int>::ValueType& item = t_map.get(m_triplets[i]);
        ASSERT_EQ(1, item.size());
        ASSERT_EQ(i, item[0]);
    }
}

TEST_F(TripletMapTest, KeyCollision) {
    const size_t N = 5;
    Triplet key = m_triplets[0];
    TripletMap<int> t_map;
    for (size_t i=0; i<N; i++) {
        t_map.insert(key, i);
    }

    ASSERT_EQ(1, t_map.size()); // only 1 key inserted
    const TripletMap<int>::ValueType& item = t_map.get(key);
    ASSERT_EQ(N, item.size());

    // Since the order of insertion is not necessarily preserved.
    // I dump the item into a vector and sort it.
    std::vector<int> item_vec(item.begin(), item.end());
    std::sort(item_vec.begin(), item_vec.end());
    for (size_t i=0; i<N; i++) {
        ASSERT_EQ(i, item_vec[i]);
    }
}
