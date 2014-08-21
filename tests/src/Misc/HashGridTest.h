#pragma once
#include <Misc/HashGrid.h>

class HashGridTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            m_grid = HashGrid::create(0.5);
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

