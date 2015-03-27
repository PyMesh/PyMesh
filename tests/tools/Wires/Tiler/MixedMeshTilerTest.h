/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <sstream>
#include <vector>

#include <WireTest.h>
#include <Wires/Tiler/MixedMeshTiler.h>
#include <Wires/WireNetwork/WireWriter.h>

#include <Mesh.h>
#include <MeshFactory.h>

class MixedMeshTilerTest : public WireTest {
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
        
        void save_wires(const std::string& wire_file, WireNetwork::Ptr wires) {
            WireWriter writer;
            writer.write(wire_file, *wires);
        }

        VectorF generate_pattern_id(size_t num_cells, size_t num_patterns) {
            VectorF pattern_id(num_cells);
            for (size_t i=0; i<num_cells; i++) {
                pattern_id[i] = (i % num_patterns);
            }
            return pattern_id;
        }

        void add_dofs_as_attributes(Mesh::Ptr mesh, const MatrixF& dofs) {
            const size_t num_dofs = dofs.cols();
            const size_t num_cells = dofs.rows();
            for (size_t i=0; i<num_dofs; i++) {
                VectorF dof = dofs.col(i);
                std::stringstream dof_name;
                dof_name << "dof_" << i;
                mesh->add_attribute(dof_name.str());
                mesh->set_attribute(dof_name.str(), dof);
            }
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

TEST_F(MixedMeshTilerTest, 2D) {
    std::vector<WireNetwork::Ptr> patterns;
    patterns.push_back(load_wire_shared("box.wire"));
    patterns.push_back(load_wire_shared("cross.wire"));

    MeshPtr mesh = load_mesh("quad_s1.obj");
    const size_t num_cells = mesh->get_num_faces();
    VectorF pattern_id = generate_pattern_id(num_cells, 2);
    mesh->add_attribute("pattern_id");
    mesh->set_attribute("pattern_id", pattern_id);

    size_t max_num_dofs = 5;
    MatrixF dofs = MatrixFr::Ones(num_cells, max_num_dofs) * 0.1;
    add_dofs_as_attributes(mesh, dofs);

    MixedMeshTiler tiler(patterns, mesh);
    WireNetwork::Ptr tiled_network = tiler.tile();

    MatrixFr offset = tiled_network->get_attribute("vertex_offset");
    tiled_network->set_vertices(tiled_network->get_vertices() + offset);

    //save_wires("mixed_wires.obj", tiled_network);

    ASSERT_BBOX_MATCHES(tiled_network->get_vertices(), mesh->get_vertices());

    size_t expected_num_vertices = (
            patterns[0]->get_num_vertices() +
            patterns[1]->get_num_vertices()) * 2 - 4;
    ASSERT_EQ(expected_num_vertices, tiled_network->get_num_vertices());
}

TEST_F(MixedMeshTilerTest, 3D) {
    std::vector<WireNetwork::Ptr> patterns;
    patterns.push_back(load_wire_shared("brick5.wire"));
    patterns.push_back(load_wire_shared("cross_3D.wire"));

    MeshPtr mesh = load_mesh("hex_s1.msh");
    const size_t num_cells = mesh->get_num_voxels();
    VectorF pattern_id = generate_pattern_id(num_cells, 2);
    mesh->add_attribute("pattern_id");
    mesh->set_attribute("pattern_id", pattern_id);

    size_t max_num_dofs = 5;
    MatrixF dofs = MatrixFr::Ones(num_cells, max_num_dofs) * 0.2;
    add_dofs_as_attributes(mesh, dofs);

    MixedMeshTiler tiler(patterns, mesh);
    WireNetwork::Ptr tiled_network = tiler.tile();

    MatrixFr offset = tiled_network->get_attribute("vertex_offset");
    tiled_network->set_vertices(tiled_network->get_vertices() + offset);

    //save_wires("mixed_wires.obj", tiled_network);

    ASSERT_BBOX_MATCHES(tiled_network->get_vertices(), mesh->get_vertices());

    size_t expected_num_vertices = (
            patterns[0]->get_num_vertices() +
            patterns[1]->get_num_vertices()) * 4 - 12;
    ASSERT_EQ(expected_num_vertices, tiled_network->get_num_vertices());
}

TEST_F(MixedMeshTilerTest, 3D_single_pattern) {
    std::vector<WireNetwork::Ptr> patterns;
    patterns.push_back(load_wire_shared("cube.wire"));

    MeshPtr mesh = load_mesh("hex_s1.msh");
    const size_t num_cells = mesh->get_num_voxels();
    VectorF pattern_id = generate_pattern_id(num_cells, 1);
    mesh->add_attribute("pattern_id");
    mesh->set_attribute("pattern_id", pattern_id);

    size_t max_num_dofs = 2;
    MatrixF dofs = MatrixFr::Ones(num_cells, max_num_dofs) * 0.2;
    for (size_t i=0; i<num_cells; i++) {
        dofs(i, 0) += i * 0.02;
    }
    add_dofs_as_attributes(mesh, dofs);

    MixedMeshTiler tiler(patterns, mesh);
    WireNetwork::Ptr tiled_network = tiler.tile();

    ASSERT_TRUE(tiled_network->has_attribute("thickness"));
    MatrixFr thickness = tiled_network->get_attribute("thickness");
    ASSERT_FALSE((thickness.array() == thickness(0, 0)).all());
}

