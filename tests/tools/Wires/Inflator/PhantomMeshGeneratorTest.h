/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WireTest.h>

#include <Wires/Inflator/PhantomMeshGenerator.h>
#include <Wires/Inflator/WireProfile.h>
#include <Wires/Parameters/ParameterManager.h>

class PhantomMeshGeneratorTest : public WireTest {
    protected:
};

TEST_F(PhantomMeshGeneratorTest, creation) {
    const Vector3F half_size(2.5, 2.5, 2.5);
    WireNetwork::Ptr network = load_wire_shared("brick5.wire");
    network->scale_fit(-half_size, half_size);

    WireProfile::Ptr profile = WireProfile::create("square");

    ParameterManager::Ptr manager = ParameterManager::create(network);

    PhantomMeshGenerator generator(network, manager, profile);
    generator.with_shape_velocities();
    generator.generate();

    Mesh::Ptr mesh = form_mesh(
            generator.get_vertices(), generator.get_faces());

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t dim = mesh->get_dim();
    size_t count=0;
    std::vector<std::string> attr_names;
    const auto& shape_velocities = generator.get_shape_velocities();
    for (const auto& shape_velocity : shape_velocities) {
        VectorF velocity(num_vertices * dim);
        std::copy(shape_velocity.data(),
                shape_velocity.data() + num_vertices * dim,
                velocity.data());
        std::stringstream attr_name;
        attr_name << "velocity_" << count;
        mesh->add_attribute(attr_name.str());
        mesh->set_attribute(attr_name.str(), velocity);
        attr_names.push_back(attr_name.str());
        count++;
    }

    save_mesh("phantom_brick5.msh", mesh, attr_names);
}
