#pragma once

#include <set>

#include <Wires/Inflator/PeriodicInflator2D.h>
#include <Wires/Inflator/WireProfile.h>
#include <Wires/Inflator/InflatorEngine.h>
#include <WireTest.h>
#include <IO/MeshWriter.h>
#include <Misc/HashGrid.h>
#include <MeshFactory.h>

class PeriodicInflator2DTest : public WireTest {
    protected:
        void save_mesh(const std::string& file_name,
                MatrixFr& vertices, MatrixIr& faces, VectorF debug) {
            VectorF flat_vertices(vertices.rows() * vertices.cols());
            std::copy(vertices.data(), vertices.data() + flat_vertices.size(), flat_vertices.data());
            VectorI flat_faces(faces.rows() * faces.cols());
            std::copy(faces.data(), faces.data() + flat_faces.size(), flat_faces.data());
            VectorI flat_voxels = VectorI::Zero(0);

            MeshFactory factory;
            Mesh::Ptr mesh = factory.load_data(flat_vertices, flat_faces, flat_voxels,
                    vertices.cols(), faces.cols(), 4).create_shared();
            mesh->add_attribute("debug");
            mesh->set_attribute("debug", debug);

            MeshWriter::create_writer(file_name)->with_attribute("debug").write_mesh(*mesh);
        }

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
    WireNetwork network = load_wire("square.wire");
    network.compute_connectivity();
    network.scale(Vector2F::Ones() * 5); // 5mm cell
    const size_t num_vertices = network.get_num_vertices();
    const size_t num_edges = network.get_num_edges();

    PeriodicInflator2D inflator(network);
    inflator.set_thickness_type(InflatorEngine::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.2);
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
    WireNetwork network = load_wire("box.wire");
    network.compute_connectivity();
    network.scale(Vector2F::Ones() * 5); // 5mm cell
    const size_t num_vertices = network.get_num_vertices();
    const size_t num_edges = network.get_num_edges();

    PeriodicInflator2D inflator(network);
    inflator.set_thickness_type(InflatorEngine::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.5);
    inflator.inflate();
    inflator.refine("simple", 1);

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
    WireNetwork network = load_wire("star_2D.wire");
    network.compute_connectivity();
    network.scale(Vector2F::Ones() * 5); // 5mm cell
    const size_t num_vertices = network.get_num_vertices();
    const size_t num_edges = network.get_num_edges();

    PeriodicInflator2D inflator(network);
    inflator.set_thickness_type(InflatorEngine::PER_EDGE);
    inflator.set_thickness(VectorF::Ones(num_edges) * 0.5);
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
}

