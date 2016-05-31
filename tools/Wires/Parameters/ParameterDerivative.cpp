/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ParameterDerivative.h"

#include <Core/Exception.h>

using namespace PyMesh;

ParameterDerivative::ParameterDerivative(Mesh::Ptr mesh, PatternParameter::Ptr param)
    : m_mesh(mesh), m_parameter(param) {
        initialize_wires();
        initialize_mesh();
        initialize_normals();
        initialize_face_voronoi_areas();
    }

void ParameterDerivative::initialize_wires()  {
    WireNetwork::Ptr wire_network = m_parameter->get_wire_network();
    if (!wire_network->with_connectivity()) {
        wire_network->compute_connectivity();
    }
}

void ParameterDerivative::initialize_mesh() {
    assert(m_mesh->get_vertex_per_face() == 3);
    if (!m_mesh->has_attribute("face_voronoi_area")) {
        m_mesh->add_attribute("face_voronoi_area");
    }

    if (!m_mesh->has_attribute("face_source")) {
        throw RuntimeError("Mesh does not have face source attribute");
    }
    m_face_source = m_mesh->get_attribute("face_source").cast<int>();
}

void ParameterDerivative::initialize_normals() {
    const size_t dim = m_mesh->get_dim();
    if (dim == 2) {
        throw NotImplementedError("2D is not supported yet");
    } else {
        if (!m_mesh->has_attribute("face_normal")) {
            m_mesh->add_attribute("face_normal");
        }
        VectorF face_normals = m_mesh->get_attribute("face_normal");
        m_face_normals.resize(m_mesh->get_num_faces(), dim);
        std::copy(face_normals.data(), face_normals.data() + face_normals.size(),
                m_face_normals.data());
    }
}

void ParameterDerivative::initialize_face_voronoi_areas() {
    VectorF face_voronoi_area = m_mesh->get_attribute("face_voronoi_area");
    m_face_voronoi_areas.resize(m_mesh->get_num_faces(),
            m_mesh->get_vertex_per_face());
    std::copy(face_voronoi_area.data(),
            face_voronoi_area.data() + face_voronoi_area.size(),
            m_face_voronoi_areas.data());
}

