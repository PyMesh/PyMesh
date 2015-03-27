/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <WireTest.h>
#include <Wires/Tiler/MeshTiler.h>
#include <Wires/Parameters/ParameterManager.h>

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

        MeshPtr scale_mesh(MeshPtr mesh, Float scale) {
            VectorF vertices = mesh->get_vertices() * scale;
            VectorI faces = mesh->get_faces();
            VectorI voxels = mesh->get_voxels();

            MeshFactory factory;
            factory.load_data(vertices, faces, voxels,
                    mesh->get_dim(),
                    mesh->get_vertex_per_face(),
                    mesh->get_vertex_per_voxel());
            return factory.create_shared();
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

TEST_F(MeshTilerTest, hex_s1) {
    WireNetwork::Ptr wire_network = load_wire_shared("brick5.wire");
    ParameterManager::Ptr manager = ParameterManager::create_isotropic(
            wire_network, 0.5);
    const size_t num_dofs = manager->get_num_dofs();
    const size_t num_thickness_dofs = manager->get_num_thickness_dofs();
    VectorF dofs = VectorF::Zero(num_dofs);
    for (size_t i=0; i<num_dofs; i++) {
        if (i < num_thickness_dofs) {
            dofs[i] = 0.5 + 0.25 * i;
        } else {
            dofs[i] = (i % 2 == 0) ? 0.2 : -0.2;
        }
    }
    manager->set_dofs(dofs);

    MeshPtr mesh = load_mesh("hex_s1.msh");
    mesh = scale_mesh(mesh, 10);
    MeshTiler tiler(wire_network, mesh);
    tiler.with_parameters(manager);
    WireNetwork::Ptr tiled_network = tiler.tile();

    ASSERT_EQ(wire_network->get_num_edges() * mesh->get_num_voxels(),
            tiled_network->get_num_edges());
    ASSERT_BBOX_MATCHES(tiled_network->get_vertices(), mesh->get_vertices());
    ASSERT_TRUE(tiled_network->has_attribute("thickness"));
    ASSERT_TRUE(tiled_network->has_attribute("vertex_offset"));

    MatrixFr offset = tiled_network->get_attribute("vertex_offset");
    VectorF bbox_max = tiled_network->get_bbox_max();
    VectorF center = tiled_network->center();
    VectorF half_bbox_size = (bbox_max - center) / 2.0;
    const size_t num_vertices = tiled_network->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        offset.row(i) = offset.row(i).cwiseQuotient(half_bbox_size.transpose());
    }
    ASSERT_FLOAT_EQ( 0.2, offset.maxCoeff());
    ASSERT_FLOAT_EQ(-0.2, offset.minCoeff());
}
