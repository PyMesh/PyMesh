/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <set>

#include <Wires/Inflator/PeriodicInflator2D.h>
#include <Wires/Inflator/WireProfile.h>
#include <Wires/Inflator/InflatorEngine.h>
#include <Wires/Parameters/ParameterManager.h>
#include <WireTest.h>
#include <IO/MeshWriter.h>
#include <Misc/HashGrid.h>
#include <MeshFactory.h>

class PeriodicInflator2DTest : public WireTest {
    protected:
        void ASSERT_PERIODIC(const MatrixFr& vertices) {
            const Float EPS = 1e-6;
            const VectorF bbox_min = vertices.colwise().minCoeff();
            const VectorF bbox_max = vertices.colwise().maxCoeff();
            const VectorF bbox_size = bbox_max - bbox_min;

            HashGrid::Ptr grid = HashGrid::create(1e-6, 2);
            const size_t num_vertices = vertices.rows();
            for (size_t i=0; i<num_vertices; i++) {
                const VectorF v = vertices.row(i);
                grid->insert(i, v);
            }

            for (size_t i=0; i<num_vertices; i++) {
                const VectorF v = vertices.row(i);
                if (fabs(v[0] - bbox_min[0]) < EPS) {
                    Vector2F other = v + Vector2F(bbox_size[0], 0);
                    VectorI candidates = grid->get_items_near_point(other);
                    ASSERT_EQ(1, candidates.size());
                }
                if (fabs(v[1] - bbox_min[1]) < EPS) {
                    Vector2F other = v + Vector2F(0, bbox_size[1]);
                    VectorI candidates = grid->get_items_near_point(other);
                    ASSERT_EQ(1, candidates.size());
                }
            }
        }
};

TEST_F(PeriodicInflator2DTest, square) {
    WireNetwork::Ptr network = load_wire_shared("square.wire");
    network->compute_connectivity();
    network->scale(Vector2F::Ones() * 5); // 5mm cell
    const size_t num_vertices = network->get_num_vertices();
    const size_t num_edges = network->get_num_edges();

    PeriodicInflator2D inflator(network);
    inflator.set_uniform_thickness(0.2);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();

    const size_t num_faces = faces.rows();
    std::set<int> base_vertex_indices;
    std::set<int> base_edge_indices;
    for (size_t i=0; i<num_faces; i++) {
        int val = face_sources[i];
        if (val < 0) {
            base_edge_indices.insert(-val - 1);
        } else if (val > 0) {
            base_vertex_indices.insert(val - 1);
        }
    }

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());

    ASSERT_EQ(num_faces, face_sources.size());

    ASSERT_EQ(1, base_vertex_indices.size());
    ASSERT_EQ(2, base_edge_indices.size());

    ASSERT_LE(0, *std::min_element(base_vertex_indices.begin(),
                base_vertex_indices.end()));
    ASSERT_LE(0, *std::min_element(base_edge_indices.begin(),
                base_edge_indices.end()));
    ASSERT_GT(num_vertices, *std::max_element(base_vertex_indices.begin(),
                base_vertex_indices.end()));
    ASSERT_GT(num_edges, *std::max_element(base_edge_indices.begin(),
                base_edge_indices.end()));

    ASSERT_PERIODIC(vertices);
}

TEST_F(PeriodicInflator2DTest, box) {
    WireNetwork::Ptr network = load_wire_shared("box.wire");
    network->compute_connectivity();
    network->scale(Vector2F::Ones() * 5); // 5mm cell
    const size_t num_vertices = network->get_num_vertices();
    const size_t num_edges = network->get_num_edges();

    PeriodicInflator2D inflator(network);
    inflator.set_uniform_thickness(0.5);
    inflator.with_refinement("simple", 1);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();
    //save_mesh("tmp2D.msh", vertices, faces,
    //        face_sources.cast<Float>());

    const size_t num_faces = faces.rows();
    std::set<int> base_vertex_indices;
    std::set<int> base_edge_indices;
    for (size_t i=0; i<num_faces; i++) {
        int val = face_sources[i];
        if (val < 0) {
            base_edge_indices.insert(-val - 1);
        } else if (val > 0) {
            base_vertex_indices.insert(val - 1);
        }
    }

    ASSERT_LT(0, vertices.rows());
    ASSERT_EQ(2, vertices.cols());
    ASSERT_LT(0, faces.rows());
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(num_faces, face_sources.size());

    //        *
    //        |
    //    +---+---+
    //    |       |
    // *--+       +--*
    //    |       |
    //    +---+---+
    //        |
    //        *
    // Note that only vertices marked with "+" would inflate to actual faces.
    // Vertices marked with "*" does not create faces.  Thus the number of
    // unique face sources is 8.
    ASSERT_EQ(8, base_vertex_indices.size());
    ASSERT_EQ(num_edges, base_edge_indices.size());

    ASSERT_LE(0, *std::min_element(base_vertex_indices.begin(),
                base_vertex_indices.end()));
    ASSERT_LE(0, *std::min_element(base_edge_indices.begin(),
                base_edge_indices.end()));
    ASSERT_GT(num_vertices, *std::max_element(base_vertex_indices.begin(),
                base_vertex_indices.end()));
    ASSERT_GT(num_edges, *std::max_element(base_edge_indices.begin(),
                base_edge_indices.end()));

    ASSERT_PERIODIC(vertices);
}

TEST_F(PeriodicInflator2DTest, star) {
    WireNetwork::Ptr network = load_wire_shared("star_2D.wire");
    network->compute_connectivity();
    network->scale(Vector2F::Ones() * 5); // 5mm cell
    const size_t num_vertices = network->get_num_vertices();
    const size_t num_edges = network->get_num_edges();

    PeriodicInflator2D inflator(network);
    inflator.set_uniform_thickness(0.5);
    inflator.inflate();

    MatrixFr vertices = inflator.get_vertices();
    MatrixIr faces = inflator.get_faces();
    VectorI  face_sources = inflator.get_face_sources();
    //save_mesh("tmp2D.msh", vertices, faces,
    //        face_sources.cast<Float>());

    const size_t num_faces = faces.rows();
    std::set<int> base_vertex_indices;
    std::set<int> base_edge_indices;
    for (size_t i=0; i<num_faces; i++) {
        int val = face_sources[i];
        if (val < 0) {
            base_edge_indices.insert(-val - 1);
        } else if (val > 0) {
            base_vertex_indices.insert(val - 1);
        }
    }

    ASSERT_EQ(2, base_vertex_indices.size());
    ASSERT_EQ(4, base_edge_indices.size());

    save_mesh("inflated_star_2D.msh", vertices, faces,
            face_sources.cast<Float>());
}

TEST_F(PeriodicInflator2DTest, box_with_parameter) {
    WireNetwork::Ptr network = load_wire_shared("box.wire");
    network->center_at_origin();
    network->compute_connectivity();
    network->scale(Vector2F::Ones() * 5); // 5mm cell

    ParameterManager::Ptr manager = ParameterManager::create_from_setting_file(
            network, 0.5,
            m_data_dir + "box.orbit",
            m_data_dir + "box.modifier");

    InflatorEngine::Ptr inflator = InflatorEngine::create_parametric(
            network, manager);
    inflator->with_refinement("simple", 2);
    inflator->inflate();

    MatrixFr vertices = inflator->get_vertices();
    MatrixIr faces = inflator->get_faces();
    VectorI face_sources = inflator->get_face_sources();

    ASSERT_GT(vertices.rows(), 0);
    ASSERT_EQ(2, vertices.cols());
    ASSERT_GT(faces.rows(), 0);
    ASSERT_EQ(3, faces.cols());
    ASSERT_EQ(faces.rows(), face_sources.size());
    save_mesh("inflated_box_params.msh", vertices, faces,
            face_sources.cast<Float>());
}

