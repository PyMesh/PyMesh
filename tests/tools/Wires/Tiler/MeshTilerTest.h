#pragma once

#include <string>

#include <WireTest.h>
#include <Wires/Tiler/MeshTiler.h>

#include <Mesh.h>
#include <MeshFactory.h>

class MeshTilerTest : public WireTest {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            WireTest::SetUp();
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_mesh_data_dir = project_dir + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& mesh_file) {
            MeshFactory factory;
            factory.load_file(m_mesh_data_dir + mesh_file);
            return factory.create_shared();
        }

        void ASSERT_BBOX_MATCHES(const MatrixFr& vts1, const VectorF& vts2_flattened) {
            const size_t dim = vts1.cols();
            Eigen::Map<const MatrixFr> vts2(vts2_flattened.data(), vts2_flattened.size() / dim, dim);
            VectorF bbox_min_1 = vts1.colwise().minCoeff();
            VectorF bbox_max_1 = vts1.colwise().maxCoeff();
            VectorF bbox_min_2 = vts2.colwise().minCoeff();
            VectorF bbox_max_2 = vts2.colwise().maxCoeff();
            ASSERT_FLOAT_EQ(0.0, (bbox_min_1 - bbox_min_2).norm());
            ASSERT_FLOAT_EQ(0.0, (bbox_max_1 - bbox_max_2).norm());
        }

    protected:
        std::string m_mesh_data_dir;
};

TEST_F(MeshTilerTest, cube) {
    WireNetwork::Ptr wire_network = load_wire_shared("cube.wire");
    MeshPtr mesh = load_mesh("hex.msh");
    MeshTiler tiler(wire_network, mesh);
    WireNetwork::Ptr tiled_network = tiler.tile();

    ASSERT_EQ(wire_network->get_num_vertices(), tiled_network->get_num_vertices());
    ASSERT_EQ(wire_network->get_num_edges(), tiled_network->get_num_edges());
    ASSERT_BBOX_MATCHES(tiled_network->get_vertices(), mesh->get_vertices());
    ASSERT_FLOAT_EQ(0.0,
            (wire_network->get_edges() -
             tiled_network->get_edges()).norm());
}

TEST_F(MeshTilerTest, square) {
    WireNetwork::Ptr wire_network = load_wire_shared("square.wire");
    MeshPtr mesh = load_mesh("quad.obj");
    MeshTiler tiler(wire_network, mesh);
    WireNetwork::Ptr tiled_network = tiler.tile();

    ASSERT_EQ(wire_network->get_num_vertices(), tiled_network->get_num_vertices());
    ASSERT_EQ(wire_network->get_num_edges(), tiled_network->get_num_edges());
    ASSERT_BBOX_MATCHES(tiled_network->get_vertices(), mesh->get_vertices());
    ASSERT_FLOAT_EQ(0.0,
            (wire_network->get_edges() -
             tiled_network->get_edges()).norm());
}

