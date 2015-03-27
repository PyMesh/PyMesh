/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Misc/HashGrid.h>

class HashGridTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            m_grid = HashGrid::create(0.5);
        }

        void init_2D() {
            m_grid = HashGrid::create(0.5, 2);
        }

        void ASSERT_IN_HASH(int obj_id, const VectorF& coord) {
            ASSERT_TRUE(m_grid->occupied(obj_id, coord));
        }

        void ASSERT_NOT_IN_HASH(int obj_id, const VectorF& coord) {
            ASSERT_FALSE(m_grid->occupied(obj_id, coord));
        }

    protected:
        HashGrid::Ptr m_grid;
};

TEST_F(HashGridTest, Empty) {
    ASSERT_EQ(0, m_grid->size());
}

TEST_F(HashGridTest, Empty2) {
    Vector3F origin = Vector3F::Zero();
    const VectorI items = m_grid->get_items_near_point(origin);
    ASSERT_EQ(0, items.size());
}

TEST_F(HashGridTest, Insert) {
    Vector3F origin = Vector3F::Zero();
    Vector3F ones = Vector3F::Ones();
    m_grid->insert(0, origin);
    m_grid->insert(1, origin);
    m_grid->insert(0, origin);
    const VectorI items = m_grid->get_items_near_point(origin);
    ASSERT_EQ(2, items.size());
    ASSERT_IN_HASH(0, origin);
    ASSERT_IN_HASH(1, origin);
    ASSERT_NOT_IN_HASH(0, ones);
    ASSERT_NOT_IN_HASH(1, ones);
    ASSERT_NOT_IN_HASH(2, origin);
}

TEST_F(HashGridTest, InsertBBox) {
    // The bbox of dotted triangle occupies 9 cells.
    // +---+---+---+
    // | . |   |   |
    // |-:-.---+---+
    // | : | . |   |
    // |-:-+---.---+
    // | :......:. |
    // +---+---+---+
    MatrixF triangle(3,3);
    triangle << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    m_grid->insert_bbox(0, triangle);
    ASSERT_EQ(9, m_grid->size());
    ASSERT_IN_HASH(0, triangle.row(0));
    ASSERT_IN_HASH(0, triangle.row(1));
    ASSERT_IN_HASH(0, triangle.row(2));
    ASSERT_IN_HASH(0, Vector3F(0.5, 0.5, 0.0));
    ASSERT_IN_HASH(0, Vector3F(0.9, 0.9, 0.0));
}

TEST_F(HashGridTest, insertTriangle) {
    // The dotted triangle occupies 8 cells.
    // +---+---+
    // | . |   |
    // |-:-.---+---+
    // | : | . |   |
    // |-:-+---.---+
    // | :......:. |
    // +---+---+---+
    MatrixF triangle(3,3);
    triangle << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    m_grid->insert_triangle(0, triangle);
    ASSERT_EQ(8, m_grid->size());
    ASSERT_IN_HASH(0, triangle.row(0));
    ASSERT_IN_HASH(0, triangle.row(1));
    ASSERT_IN_HASH(0, triangle.row(2));
    ASSERT_IN_HASH(0, Vector3F(0.5, 0.5, 0.0));
    ASSERT_NOT_IN_HASH(0, Vector3F(0.9, 0.9, 0.0));
}

TEST_F(HashGridTest, insertTriangle_2D) {
    // The dotted triangle occupies 8 cells.
    // +---+---+
    // | . |   |
    // |-:-.---+---+
    // | : | . |   |
    // |-:-+---.---+
    // | :......:. |
    // +---+---+---+
    init_2D();
    MatrixF triangle(3,2);
    triangle << 0.0, 0.0,
                1.0, 0.0,
                0.0, 1.0;
    m_grid->insert_triangle(0, triangle);
    ASSERT_EQ(8, m_grid->size());
    ASSERT_IN_HASH(0, triangle.row(0));
    ASSERT_IN_HASH(0, triangle.row(1));
    ASSERT_IN_HASH(0, triangle.row(2));
    ASSERT_IN_HASH(0, Vector2F(0.5, 0.5));
    ASSERT_NOT_IN_HASH(0, Vector2F(0.9, 0.9));
}

TEST_F(HashGridTest, InsertBatch) {
    MatrixF triangle(3,3);
    triangle << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    m_grid->insert_batch(0, triangle);
    ASSERT_EQ(3, m_grid->size());
    ASSERT_IN_HASH(0, triangle.row(0));
    ASSERT_IN_HASH(0, triangle.row(1));
    ASSERT_IN_HASH(0, triangle.row(2));
    ASSERT_NOT_IN_HASH(0, Vector3F(0.5, 0.5, 0.0));
}

TEST_F(HashGridTest, InsertMultiple) {
    MatrixF triangle(3,3);
    triangle << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    Vector3I idx(1,2,3);
    m_grid->insert_multiple(idx, triangle);
    ASSERT_EQ(3, m_grid->size());
    ASSERT_IN_HASH(idx[0], triangle.row(0));
    ASSERT_IN_HASH(idx[1], triangle.row(1));
    ASSERT_IN_HASH(idx[2], triangle.row(2));
}

TEST_F(HashGridTest, Remove) {
    Vector3F origin = Vector3F::Zero();
    Vector3F ones = Vector3F::Ones();
    m_grid->insert(0, origin);
    m_grid->insert(1, origin);
    m_grid->insert(2, origin);
    m_grid->remove(2, origin);
    const VectorI items = m_grid->get_items_near_point(origin);
    ASSERT_EQ(2, items.size());
    ASSERT_IN_HASH(0, origin);
    ASSERT_IN_HASH(1, origin);
    ASSERT_NOT_IN_HASH(0, ones);
    ASSERT_NOT_IN_HASH(1, ones);
    ASSERT_NOT_IN_HASH(2, origin);
}

TEST_F(HashGridTest, GetItems) {
    Vector3F origin = Vector3F::Zero();
    Vector3F one = Vector3F::Ones();
    m_grid->insert(0, origin);
    m_grid->insert(1, origin);
    m_grid->insert(2, origin);
    VectorI near_origin = m_grid->get_items_near_point(origin);
    std::vector<int> near_origin_vec(near_origin.data(),
            near_origin.data() + near_origin.size());
    ASSERT_EQ(3, near_origin.size());
    ASSERT_THAT(near_origin_vec, Contains(0));
    ASSERT_THAT(near_origin_vec, Contains(1));
    ASSERT_THAT(near_origin_vec, Contains(2));
    VectorI near_one = m_grid->get_items_near_point(one);
    ASSERT_EQ(0, near_one.size());
}

TEST_F(HashGridTest, CloseByPoints) {
    const Float eps = 1e-6;
    Vector3F origin = Vector3F::Zero();
    Vector3F one = Vector3F::Ones();
    Vector3F p1 = one - one * eps;
    Vector3F p2 = one + one * eps;
    m_grid->insert(1, p1);
    m_grid->insert(2, p2);
    VectorI near_one = m_grid->get_items_near_point(one);
    std::vector<int> near_one_vec(near_one.data(),
            near_one.data() + near_one.size());
    ASSERT_EQ(2, near_one.size());
    ASSERT_THAT(near_one_vec, Contains(1));
    ASSERT_THAT(near_one_vec, Contains(2));
}

TEST_F(HashGridTest, InsertMultipleTriangles) {
    // The dotted triangle occupies 8 cells.
    // +---+---+---+
    // | ......... |
    // |-:-.---+-:-+
    // | : | . | : |
    // |-:-+---.-:-+
    // | :......:. |
    // +---+---+---+
    MatrixF triangles(6,3);
    triangles << 0.0, 0.0, 0.0,
                 1.0, 0.0, 0.0,
                 0.0, 1.0, 0.0,
                 1.0, 0.0, 0.0,
                 1.0, 1.0, 0.0,
                 0.0, 1.0, 0.0;
    Vector2I ids(1, 2);
    m_grid->insert_multiple_triangles(ids, triangles);
    VectorF p1 = triangles.block(0, 0, 3, 3).colwise().sum() / 3.0;
    VectorF p2 = triangles.block(3, 0, 3, 3).colwise().sum() / 3.0;
    VectorF p3 = Vector3F(0.5, 0.5, 0.0);

    VectorI near_p1 = m_grid->get_items_near_point(p1);
    VectorI near_p2 = m_grid->get_items_near_point(p2);
    VectorI near_p3 = m_grid->get_items_near_point(p3);

    ASSERT_LT(0, near_p1.size());
    ASSERT_LT(0, near_p2.size());
    ASSERT_LT(0, near_p3.size());
}

TEST_F(HashGridTest, InsertMultipleTriangles_2) {
    m_grid = HashGrid::create(1e-1);
    MatrixF triangles(3, 3);
    triangles <<-0.25,2.02022106,-0.25,
                -0.25,2.02022106, 0.25,
                -0.25,2.500001  ,-0.25;
    VectorI ids(1); ids << 1;
    bool success = m_grid->insert_multiple_triangles(ids, triangles);
    ASSERT_TRUE(success);
    VectorF p1 = triangles.colwise().sum() / 3.0;
    VectorF p2 = Vector3F(-0.25, 2.1801477, -0.08333333);

    VectorI near_id_1 = m_grid->get_items_near_point(p1);
    VectorI near_id_2 = m_grid->get_items_near_point(p2);
    ASSERT_EQ(1, near_id_1.size());
    ASSERT_EQ(1, near_id_1[0]);
    ASSERT_EQ(1, near_id_2.size());
    ASSERT_EQ(1, near_id_2[0]);
}

