/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Misc/HashKey.h>

class HashKeyTest : public ::testing::Test {
    protected:
        typedef VectorHashKey<int, 2> HashKey2D;
        typedef VectorHashKey<int, 3> HashKey3D;
};

TEST_F(HashKeyTest, Creation2D) {
    HashKey2D key1(2);
    HashKey2D key2({2, 2});
    HashKey2D key3(VectorI::Ones(2) * 2);

    ASSERT_TRUE(key1.get_raw_data() == key2.get_raw_data());
    ASSERT_TRUE(key1.get_raw_data() == key3.get_raw_data());
}

TEST_F(HashKeyTest, Creation3D) {
    HashKey3D key1(10);
    HashKey3D key2({10, 10, 10});
    HashKey3D key3(Vector3I(10, 10, 10));

    ASSERT_TRUE(key1.get_raw_data() == key2.get_raw_data());
    ASSERT_TRUE(key1.get_raw_data() == key3.get_raw_data());
}

TEST_F(HashKeyTest, OrderMatters2D) {
    HashKey2D key1({1, 2});
    HashKey2D key2({2, 1});
    ASSERT_NE(key1.hash(), key2.hash());
}

TEST_F(HashKeyTest, OrderMatters3D) {
    Float cell_size = 0.03;
    Vector3F p1(4.64644561, 4.75, -5.0);
    Vector3F p2(4.75, 4.64644561, -5.0);

    HashKey3D key1((p1/cell_size).template cast<int>());
    HashKey3D key2((p2/cell_size).template cast<int>());
    ASSERT_NE(key1.hash(), key2.hash());
}

TEST_F(HashKeyTest, ZCoordinate) {
    HashKey3D key1({0, 0, 1});
    HashKey3D key2({0, 0, 2});
    ASSERT_NE(key1.hash(), key2.hash());
}
