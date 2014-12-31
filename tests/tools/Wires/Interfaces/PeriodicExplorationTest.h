#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <Mesh.h>
#include <WireTest.h>
#include <Wires/Interfaces/PeriodicExploration.h>
#include <Wires/Inflator/AABBTree.h>

class PeriodicExplorationTest : public WireTest {
    protected:
        void ASSERT_ASSENDING_DIRECTION(const MatrixFr& delta_y, const MatrixFr& grad) {
            Float proj_len = delta_y.cwiseProduct(grad).sum();
            ASSERT_LT(0.0, proj_len);
        }

        Float compute_hausdorff_distance(
                const MatrixFr& vertices_1,
                const MatrixIr& faces_1,
                const MatrixFr& vertices_2) {
            AABBTree tree_1(vertices_1, faces_1);

            VectorF squared_dist;
            VectorI closest_face_indices;

            tree_1.look_up(vertices_2, squared_dist, closest_face_indices);
            return squared_dist.maxCoeff();
        }
};

TEST_F(PeriodicExplorationTest, brick5) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.25);
    explorer.with_parameters(
            m_data_dir + "brick5.orbit",
            m_data_dir + "brick5.modifier");
    explorer.with_refinement("loop", 2);

    std::cout << "This might take a few minutes ";
    std::cout.flush();
    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        Float factor = 1.0 - i * 0.1;
        VectorF modified_dofs(dofs);
        for (size_t j=0; j<num_dofs; j++) {
            if (explorer.is_thickness_dof(j)) {
                modified_dofs[j] *= factor;
            } else {
                if (dofs[j] < 0) {
                    modified_dofs[j] += i * 0.05;
                } else {
                    modified_dofs[j] -= i * 0.05;
                }
            }
        }
        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate();
        bool tetgen_success = explorer.run_tetgen();
        ASSERT_TRUE(tetgen_success);

        Mesh::Ptr mesh = explorer.get_mesh();
        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);
        }

        std::stringstream sin;
        sin << "exploration_brick5_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;;
}

TEST_F(PeriodicExplorationTest, diamond) {
    PeriodicExploration explorer(
            m_data_dir + "diamond.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("loop", 2);

    std::cout << "This might take a few minutes ";
    std::cout.flush();
    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        Float factor = 1.0 + i * 0.1;
        VectorF modified_dofs(dofs);
        for (size_t j=0; j<num_dofs; j++) {
            if (explorer.is_thickness_dof(j)) {
                modified_dofs[j] *= factor;
            } else {
                modified_dofs[j] -= i * 0.05;
            }
        }
        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate();
        bool tetgen_success = explorer.run_tetgen();

        Mesh::Ptr mesh = explorer.get_mesh();
        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);
        }

        std::stringstream sin;
        sin << "exploration_diamond_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;
}

TEST_F(PeriodicExplorationTest, finite_difference) {
    std::cout << "This might take a few minutes ";
    std::cout.flush();

    Float eps = 0.1;
    PeriodicExploration explorer(
            m_data_dir + "diamond.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("loop", 0);

    size_t num_dofs = explorer.get_num_dofs();
    VectorF dofs = explorer.get_dofs();
    for (size_t i=0; i<num_dofs; i++) {
        if (!explorer.is_thickness_dof(i)) {
            dofs[i] = 0.1;
        }
    }
    explorer.set_dofs(dofs);
    explorer.periodic_inflate();

    MatrixFr init_vertices = explorer.get_vertices();
    MatrixIr init_faces = explorer.get_faces();
    auto shape_velocities = explorer.get_shape_velocities();

    for (size_t i=0; i<num_dofs; i++) {
        VectorF modified_dofs = dofs;
        if (explorer.is_thickness_dof(i)) {
            eps = 1.0;
        } else {
            eps = 0.1;
        }
        modified_dofs[i] += eps;

        explorer.set_dofs(modified_dofs);
        explorer.periodic_inflate();

        MatrixFr vertices = explorer.get_vertices();
        MatrixIr faces = explorer.get_faces();
        MatrixFr perturbed_vertices = init_vertices + shape_velocities[i] * eps;

        std::stringstream sin_1;
        sin_1 << "diamond_perturbation_" << i << ".msh";
        save_mesh(sin_1.str(), perturbed_vertices, init_faces);

        std::stringstream sin_2;
        sin_2 << "diamond_actual_" << i << ".msh";
        save_mesh(sin_2.str(), vertices, faces);

        Float dist = compute_hausdorff_distance(
                perturbed_vertices, init_faces, vertices);
        ASSERT_LT(dist, eps);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;
}

TEST_F(PeriodicExplorationTest, gradient_descent) {
    std::cout << "This might take a few minutes ";
    std::cout.flush();

    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.25);
    explorer.with_all_isotropic_parameters();
    explorer.with_refinement("loop", 2);

    const size_t num_dofs = explorer.get_num_dofs();
    const size_t num_steps = 5;
    const Float step_size = 0.01;
    for (size_t i=0; i<num_steps; i++) {

        explorer.periodic_inflate();
        bool tetgen_success = explorer.run_tetgen();
        ASSERT_TRUE(tetgen_success);

        Mesh::Ptr mesh = explorer.get_mesh();

        mesh->add_attribute("vertex_normal");
        VectorF vertex_normals = mesh->get_attribute("vertex_normal");
        ASSERT_EQ(mesh->get_num_vertices() * mesh->get_dim(), vertex_normals.size());

        mesh->add_attribute("vertex_area");
        VectorF vertex_areas = mesh->get_attribute("vertex_area");
        ASSERT_EQ(mesh->get_num_vertices(), vertex_areas.size());

        std::vector<MatrixFr> velocities = explorer.get_shape_velocities();
        ASSERT_EQ(explorer.get_num_dofs(), velocities.size());

        VectorF grad = VectorF::Zero(num_dofs);
        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocities[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);

            const size_t num_vertices = mesh->get_num_vertices();
            for (size_t k=0; k<num_vertices; k++) {
                grad[j] += vertex_normals.segment(k*3,3).dot(
                            flattened_attr.segment(k*3,3)) * vertex_areas[k];
            }
        }

        std::stringstream sin;
        sin << "exploration_isotropic_brick5_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);

        std::stringstream dof_stream;
        dof_stream << "exploration_isotropic_brick5_itr_" << i << ".dof";
        explorer.save_dofs(dof_stream.str());

        VectorF dofs = explorer.get_dofs();
        dofs += grad * step_size;
        explorer.set_dofs(dofs);

        std::cout << ".";
        std::cout.flush();
    }
    std::cout << " done!" << std::endl;
}
