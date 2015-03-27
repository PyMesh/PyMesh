/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>

#include <Misc/Triplet.h>

TEST(TripletTest, Equality) {
    Triplet t1(1, 2, 3);
    Triplet t2(3, 2, 1);
    ASSERT_EQ(t1, t2);
}

TEST(TripletTest, Equality2) {
    Triplet t1(1, 1, 1);
    Triplet t2(1, 1, 1);
    ASSERT_EQ(t1, t2);
}

TEST(TripletTest, LessThan) {
    Triplet t1(1, 2, 3);
    Triplet t2(4, 0, 0);
    ASSERT_LT(t1, t2);
}

TEST(TripletTest, LessThan2) {
    Triplet t1(1, 2, 3);
    Triplet t2(3, 0, 0);
    ASSERT_LT(t2, t1);
}

TEST(TripletTest, DataRecovery) {
    Triplet t1(1, 2, 3);
    Vector3I data = t1.get_ori_data();
    ASSERT_EQ(1, data[0]);
    ASSERT_EQ(2, data[1]);
    ASSERT_EQ(3, data[2]);
}

TEST(TripletTest, DataRecovery2) {
    Triplet t1(5, 4, 3);
    Vector3I data = t1.get_data();
    Triplet t2(data[0], data[1], data[2]);
    ASSERT_EQ(t1, t2);
}
