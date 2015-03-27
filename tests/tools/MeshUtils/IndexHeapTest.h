/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <MeshUtils/IndexHeap.h>

class IndexHeapTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            N = 10;
            for (size_t i=0; i<N; i++) {
                f_data.push_back(Float(i));
                i_data.push_back(i);
            }
        }

    protected:
        template<typename T>
        void check_order(IndexHeap<T>& heap, bool desc_order) {
            ASSERT_FALSE(heap.empty());

            T prev = heap.top_value();
            while(!heap.empty()) {
                T curr = heap.top_value();
                if (desc_order) {
                    ASSERT_GE(prev, curr);
                } else {
                    ASSERT_LE(prev, curr);
                }
                prev = curr;
                heap.pop();
            }
        }

    protected:
        size_t N;
        std::vector<Float> f_data;
        std::vector<int>   i_data;
};

TEST_F(IndexHeapTest, Empty) {
    IndexHeap<int> heap;
    ASSERT_TRUE(heap.empty());
}

TEST_F(IndexHeapTest, Size) {
    IndexHeap<int> i_heap(i_data);
    ASSERT_FALSE(i_heap.empty());
    ASSERT_EQ(N, i_heap.size());

    IndexHeap<Float> f_heap(f_data);
    ASSERT_FALSE(f_heap.empty());
    ASSERT_EQ(N, f_heap.size());
}

TEST_F(IndexHeapTest, TopConsistency) {
    IndexHeap<int> i_heap(i_data);
    ASSERT_LT(i_heap.top(), N);
    ASSERT_EQ(i_data[i_heap.top()], i_heap.top_value());

    IndexHeap<Float> f_heap(f_data);
    ASSERT_LT(f_heap.top(), N);
    ASSERT_EQ(f_data[f_heap.top()], f_heap.top_value());
}

TEST_F(IndexHeapTest, TopValue) {
    const int max_i = *std::max_element(i_data.begin(), i_data.end());
    const int min_i = *std::min_element(i_data.begin(), i_data.end());
    const Float max_f = *std::max_element(f_data.begin(), f_data.end());
    const Float min_f = *std::min_element(f_data.begin(), f_data.end());

    IndexHeap<int> i_heap_max(i_data, true);
    ASSERT_EQ(max_i, i_heap_max.top_value());
    IndexHeap<int> i_heap_min(i_data, false);
    ASSERT_EQ(min_i, i_heap_min.top_value());

    IndexHeap<Float> f_heap_max(f_data, true);
    ASSERT_EQ(max_f, f_heap_max.top_value());
    IndexHeap<Float> f_heap_min(f_data, false);
    ASSERT_EQ(min_f, f_heap_min.top_value());
}

TEST_F(IndexHeapTest, Order) {
    IndexHeap<int> i_heap_max(i_data, true);
    check_order(i_heap_max, true);

    IndexHeap<int> i_heap_min(i_data, false);
    check_order(i_heap_min, false);

    IndexHeap<Float> f_heap_max(f_data, true);
    check_order(f_heap_max, true);

    IndexHeap<Float> f_heap_min(f_data, false);
    check_order(f_heap_min, false);
}

TEST_F(IndexHeapTest, ReInit) {
    std::vector<int> i_data_new(1, 100);
    IndexHeap<int> i_heap(i_data);
    i_heap.init(i_data_new);
    ASSERT_EQ(1, i_heap.size());
    ASSERT_EQ(100, i_heap.top_value());

    std::vector<Float> f_data_new(1, 100.0);
    IndexHeap<Float> f_heap(f_data);
    f_heap.init(f_data_new);
    ASSERT_EQ(1, f_heap.size());
    ASSERT_FLOAT_EQ(100.0, f_heap.top_value());
}

