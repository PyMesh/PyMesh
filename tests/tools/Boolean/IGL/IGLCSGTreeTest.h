/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Boolean/CSGTree.h>
#include "../BooleanEngineTest.h"

class IGLCSGTreeTest : public BooleanEngineTest {
    protected:
        BooleanPtr get_epsilon_box_setting(
                const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p, const Float EPS=0.1) {
            MatrixFr box_vertices;
            MatrixIr box_faces;
            generate_epsilon_box(p, box_vertices, box_faces, EPS);

            BooleanPtr igl_engine = BooleanEngine::create("igl");
            igl_engine->set_mesh_1(vertices, faces);
            igl_engine->set_mesh_2(box_vertices, box_faces);
            igl_engine->compute_intersection();
            return igl_engine;
        }

        void assert_interior(const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p) {
            const Float EPS = 0.1;
            BooleanPtr igl_engine = get_epsilon_box_setting(vertices, faces, p, EPS);

            const MatrixFr& vts = igl_engine->get_vertices();
            VectorF bbox_max = vts.colwise().maxCoeff();
            VectorF bbox_min = vts.colwise().minCoeff();
            VectorF bbox_size = bbox_max - bbox_min;

            ASSERT_FLOAT_EQ(EPS*2, bbox_size.maxCoeff());
            ASSERT_FLOAT_EQ(EPS*2, bbox_size.minCoeff());
        }

        void assert_on_boundary(const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p) {
            const Float EPS = 0.1;
            BooleanPtr igl_engine = get_epsilon_box_setting(vertices, faces, p, EPS);

            const MatrixFr& vts = igl_engine->get_vertices();
            VectorF bbox_max = vts.colwise().maxCoeff();
            VectorF bbox_min = vts.colwise().minCoeff();
            VectorF bbox_size = bbox_max - bbox_min;

            // The intersection of epsilon box with the given shape would
            // 1. cut off one or more corners of the epsilon box, in which case
            //    the number of verticing should be > 8.
            // 2. if the number of vertices <= 8, then the bbox of the
            //    intersection must be smaller than the epsilon box.
            ASSERT_TRUE(vts.rows() > 8 || EPS*2 > bbox_size.minCoeff());
        }

        void assert_exterior(const MatrixFr& vertices, const MatrixIr& faces,
                const VectorF& p) {
            const Float EPS = 0.1;
            BooleanPtr igl_engine = get_epsilon_box_setting(vertices, faces, p, EPS);
            ASSERT_EQ(0, igl_engine->get_vertices().rows());
        }

        void generate_epsilon_box(const VectorF& p,
                MatrixFr& vertices, MatrixIr& faces, Float epsilon) {
            const size_t dim = p.size();
            if (dim == 3) {
                vertices.resize(8, dim);
                vertices << p[0]-epsilon, p[1]-epsilon, p[2]-epsilon,
                            p[0]+epsilon, p[1]-epsilon, p[2]-epsilon,
                            p[0]+epsilon, p[1]+epsilon, p[2]-epsilon,
                            p[0]-epsilon, p[1]+epsilon, p[2]-epsilon,
                            p[0]-epsilon, p[1]-epsilon, p[2]+epsilon,
                            p[0]+epsilon, p[1]-epsilon, p[2]+epsilon,
                            p[0]+epsilon, p[1]+epsilon, p[2]+epsilon,
                            p[0]-epsilon, p[1]+epsilon, p[2]+epsilon;
                faces.resize(12, 3);
                faces << 0, 3, 1,
                         1, 3, 2,
                         4, 5, 7,
                         7, 5, 6,
                         5, 1, 2,
                         5, 2, 6,
                         4, 7, 3,
                         4, 3, 0,
                         0, 1, 5,
                         0, 5, 4,
                         2, 3, 6,
                         3, 7, 6;
            } else {
                throw NotImplementedError("Only 3D epsilon box is supported.");
            }
        }
};

TEST_F(IGLCSGTreeTest, leaf) {
    MeshPtr mesh = load_mesh("cube.obj");
    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    CSGTree::Ptr tree = CSGTree::create_leaf("igl", vertices, faces);

    const auto r_vertices = tree->get_vertices();
    const auto r_faces = tree->get_faces();

    ASSERT_EQ(vertices.rows(), r_vertices.rows());
    ASSERT_EQ(vertices.cols(), r_vertices.cols());
    ASSERT_FLOAT_EQ(0.0, (vertices - r_vertices).minCoeff());
    ASSERT_FLOAT_EQ(0.0, (vertices - r_vertices).maxCoeff());
}

TEST_F(IGLCSGTreeTest, binary_union) {
    MeshPtr mesh = load_mesh("cube.obj");
    MatrixFr vertices_1 = extract_vertices(mesh);
    MatrixIr faces_1 = extract_faces(mesh);

    MatrixFr vertices_2 = vertices_1;
    vertices_2.col(0) += VectorF::Ones(mesh->get_num_vertices());
    MatrixIr faces_2 = faces_1;

    CSGTree::Ptr tree_1 = CSGTree::create_leaf("igl", vertices_1, faces_1);
    CSGTree::Ptr tree_2 = CSGTree::create_leaf("igl", vertices_2, faces_2);
    CSGTree::Ptr tree = CSGTree::create("igl");
    tree->set_operand_1(tree_1);
    tree->set_operand_2(tree_2);
    tree->compute_union();

    const auto r_vertices = tree->get_vertices();
    const auto r_faces = tree->get_faces();

    VectorF origin_1 = VectorF::Zero(3);
    VectorF origin_2 = Vector3F(1, 0, 0);
    Vector3F border(2, 0, 0);

    assert_interior(r_vertices, r_faces, origin_1);
    assert_interior(r_vertices, r_faces, origin_2);
    assert_on_boundary(r_vertices, r_faces, border);
}

TEST_F(IGLCSGTreeTest, nary_union) {
    MeshPtr mesh = load_mesh("cube.obj");
    MatrixFr vertices = extract_vertices(mesh);
    MatrixIr faces = extract_faces(mesh);

    CSGTree::Ptr tree = CSGTree::create_leaf("igl", vertices, faces);
    const size_t N = 10;
    for (size_t i=0; i<N; i++) {
        vertices.col(0) += VectorF::Ones(mesh->get_num_vertices());
        CSGTree::Ptr root = CSGTree::create("igl");
        root->set_operand_1(tree);
        root->set_operand_2(CSGTree::create_leaf("igl", vertices, faces));
        root->compute_union();
        tree = root;
    }

    const auto r_vertices = tree->get_vertices();
    const auto r_faces = tree->get_faces();

    Vector3F left_end(-1, 0, 0);
    Vector3F right_end(1+N, 0, 0);

    assert_on_boundary(r_vertices, r_faces, left_end);
    assert_on_boundary(r_vertices, r_faces, right_end);
}
