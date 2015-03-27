/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <sstream>
#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>
#include <WireTest.h>
#include <Wires/Inflator/InflatorEngine.h>
#include <Wires/Parameters/ParameterManager.h>

class ParameterDerivativeTest : public WireTest {
    protected:
        Mesh::Ptr inflate(
                WireNetwork::Ptr wire_network,
                ParameterManager::Ptr manager,
                bool use_reflective_inflator=true) {
            InflatorEngine::Ptr inflator;
            if (use_reflective_inflator) {
                inflator = InflatorEngine::create_isotropic_parametric(wire_network, manager);
            } else {
                inflator = InflatorEngine::create_parametric(wire_network, manager);
            }
            inflator->with_shape_velocities();
            inflator->inflate();

            MatrixFr vertices = inflator->get_vertices();
            MatrixIr faces = inflator->get_faces();
            VectorF face_sources = inflator->get_face_sources().cast<Float>();
            const auto shape_velocities = inflator->get_shape_velocities();

            Mesh::Ptr mesh = form_mesh(vertices, faces);
            mesh->add_attribute("face_source");
            mesh->set_attribute("face_source", face_sources);

            size_t count = 0;
            for (const auto& velocity : shape_velocities) {
                std::stringstream attr_name;
                VectorF value = MatrixUtils::flatten<VectorF>(velocity);
                attr_name << "shape_velocity_" << count;
                mesh->add_attribute(attr_name.str());
                mesh->set_attribute(attr_name.str(), value);
            }
            return mesh;
        }
};
