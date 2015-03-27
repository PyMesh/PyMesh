/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Attributes/WireVertexPeriodicIndexAttribute.h>
#include <WireTest.h>

class WireVertexPeriodicIndexAttributeTest : public WireTest {
    protected:
        void ASSERT_PERIODICITY(const MatrixFr& vertices,
                const MatrixFr& periodic_index) {
            const size_t dim = vertices.cols();
            const VectorF bbox_min = vertices.colwise().minCoeff();
            const VectorF bbox_max = vertices.colwise().maxCoeff();
            const VectorF bbox_size = bbox_max - bbox_min;

            ASSERT_EQ(periodic_index.size(), vertices.rows());
            const size_t num_vertices = vertices.rows();
            for (size_t i=0; i<num_vertices; i++) {
                ASSERT_LT(periodic_index(i, 0), num_vertices);
                size_t j = periodic_index(i, 0);
                if (i == j) continue;
                const VectorF& vi = vertices.row(i);
                const VectorF& vj = vertices.row(j);
                VectorF gap = (vi - vj).array().abs();
                for (size_t k=0; k<dim; k++) {
                    if (gap[k] != 0.0) {
                        ASSERT_FLOAT_EQ(bbox_size[k], gap[k]);
                    }
                }
            }
        }
};

TEST_F(WireVertexPeriodicIndexAttributeTest, cube) {
    WireNetwork network = load_wire("cube.wire");
    WireVertexPeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());
}

TEST_F(WireVertexPeriodicIndexAttributeTest, square) {
    WireNetwork network = load_wire("square.wire");
    WireVertexPeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_FLOAT_EQ(0.0, values.minCoeff());
    ASSERT_FLOAT_EQ(0.0, values.maxCoeff());
}

TEST_F(WireVertexPeriodicIndexAttributeTest, brick5) {
    WireNetwork network = load_wire("brick5.wire");
    WireVertexPeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_PERIODICITY(network.get_vertices(), values);
}

TEST_F(WireVertexPeriodicIndexAttributeTest, box) {
    WireNetwork network = load_wire("box.wire");
    WireVertexPeriodicIndexAttribute attr;
    attr.compute(network);
    const MatrixFr& values = attr.get_values();

    ASSERT_EQ(network.get_num_vertices(), values.rows());
    ASSERT_EQ(1, values.cols());
    ASSERT_PERIODICITY(network.get_vertices(), values);
}
