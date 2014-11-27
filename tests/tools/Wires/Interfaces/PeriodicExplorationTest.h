#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <Mesh.h>
#include <WireTest.h>
#include <Wires/Interfaces/PeriodicExploration.h>

class PeriodicExplorationTest : public WireTest {
};

TEST_F(PeriodicExplorationTest, DISABLED_brick5) {
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
        std::cout << ".";
        std::cout.flush();
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
    }
    std::cout << " done!" << std::endl;;
}

TEST_F(PeriodicExplorationTest, diamond) {
    PeriodicExploration explorer(
            m_data_dir + "diamond.wire", 5, 0.5);
    explorer.with_all_parameters();
    explorer.with_refinement("simple", 2);

    std::cout << "This might take a few minutes ";
    std::cout.flush();
    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        std::cout << ".";
        std::cout.flush();
        Float factor = 1.0 + i * 0.1;
        VectorF modified_dofs(dofs);
        for (size_t j=0; j<num_dofs; j++) {
            if (explorer.is_thickness_dof(j)) {
                modified_dofs[j] *= factor;
            } else {
                modified_dofs[j] -= i * 0.1;
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
    }
    std::cout << " done!" << std::endl;
}
