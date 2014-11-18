#pragma once

#include <string>
#include <sstream>
#include <vector>

#include <Mesh.h>
#include <WireTest.h>
#include <Wires/Interfaces/PeriodicExploration.h>

class PeriodicExplorationTest : public WireTest {
};

TEST_F(PeriodicExplorationTest, creation) {
    PeriodicExploration explorer(
            m_data_dir + "brick5.wire", 5, 0.5);
    explorer.with_parameters(
            m_data_dir + "brick5.orbit",
            m_data_dir + "brick5.modifier");

    VectorF dofs = explorer.get_dofs();
    const size_t num_dofs = explorer.get_num_dofs();
    for (size_t i=0; i<5; i++) {
        Float factor = 1.0 - i * 0.1;
        explorer.set_dofs(dofs * factor);
        explorer.periodic_inflate();

        Mesh::Ptr mesh = explorer.get_mesh();
        std::vector<MatrixFr> velocity = explorer.get_shape_velocity();
        ASSERT_EQ(explorer.get_num_dofs(), velocity.size());

        std::vector<std::string> attr_names;
        for (size_t j=0; j<num_dofs; j++) {
            std::stringstream attr_name_stream;
            attr_name_stream << "velocity_" << j;
            std::string attr_name = attr_name_stream.str();

            VectorF flattened_attr = flatten(velocity[j]);

            mesh->add_attribute(attr_name);
            mesh->set_attribute(attr_name, flattened_attr);
            attr_names.push_back(attr_name);
        }

        std::stringstream sin;
        sin << "exploration_itr_" << i << ".msh";
        save_mesh(sin.str(), mesh, attr_names);
    }
}
