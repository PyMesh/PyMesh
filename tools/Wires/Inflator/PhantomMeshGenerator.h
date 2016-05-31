/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>
#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Parameters/ParameterManager.h>
#include "WireProfile.h"

namespace PyMesh {

class PhantomMeshGenerator {
    public:
        PhantomMeshGenerator(
                WireNetwork::Ptr wire_network,
                ParameterManager::Ptr manager,
                WireProfile::Ptr profile) :
            m_wire_network(wire_network),
            m_parameter_manager(manager),
            m_profile(profile),
            m_with_shape_velocities(false) {
                const size_t dim = m_wire_network->get_dim();
                m_rel_geometry_correction = VectorF::Zero(dim);
                m_abs_geometry_correction = VectorF::Zero(dim);
                m_geometry_correction_cap = 0.3;
            }

    public:
        void with_shape_velocities() { m_with_shape_velocities = true; }
        void with_rel_geometry_correction(const VectorF& correction) {
            m_rel_geometry_correction = correction;
        }
        void with_abs_geometry_correction(const VectorF& correction) {
            m_abs_geometry_correction = correction;
        }
        void set_geometry_correction_cap(Float cap) {
            m_geometry_correction_cap = cap;
        }
        void generate();

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI get_face_sources() const { return m_face_sources_from_ori_wires; }

        std::vector<MatrixFr> get_shape_velocities() const;

    private:
        void initialize_wire_network();
        void convert_parameters_to_attributes();
        void tile();
        void trim_irrelavent_edges();
        void convert_attributes_to_parameters();
        void inflate();
        void update_face_sources(const VectorI& face_sources);
        void compute_phantom_shape_velocities();

        void save_mesh(
                const std::string& filename,
                const MatrixFr& vertices,
                const MatrixIr& faces,
                VectorF debug);

    private:
        WireNetwork::Ptr m_wire_network;
        WireNetwork::Ptr m_phantom_wires;

        WireProfile::Ptr m_profile;

        ParameterManager::Ptr m_parameter_manager;
        ParameterManager::Ptr m_phantom_param_manager;

        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_sources_from_ori_wires;
        VectorI  m_face_sources_from_phantom_wires;

        bool m_with_shape_velocities;
        std::vector<MatrixFr> m_shape_velocities;

        std::vector<std::string> m_thickness_roi_attr_names;
        std::vector<std::string> m_offset_roi_attr_names;
        std::vector<std::string> m_offset_derivative_attr_names;

        VectorF m_rel_geometry_correction;
        VectorF m_abs_geometry_correction;
        Float m_geometry_correction_cap;
};

}
