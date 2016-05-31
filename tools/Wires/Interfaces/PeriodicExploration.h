/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Inflator/WireProfile.h>

namespace PyMesh {

class PeriodicExploration {
    public:
        PeriodicExploration(const std::string& wire_file,
                Float cell_size,
                Float default_thickness);

        void with_parameters(
                const std::string& orbit_file,
                const std::string& modifier_file);

        typedef ParameterCommon::TargetType TargetType;
        void with_all_parameters(
                TargetType thickness_type = ParameterCommon::VERTEX);
        void with_all_isotropic_parameters(
                TargetType thickness_type = ParameterCommon::VERTEX);

        void with_refinement(
                const std::string& algorithm,
                size_t order);

        void with_profile(const std::string& profile_name);
        void with_profile(size_t num_samples);

    public:
        size_t get_num_dofs() const {
            return m_parameters->get_num_dofs();
        }

        const VectorF get_dofs() const {
            return m_parameters->get_dofs();
        }

        void set_dofs(const VectorF& val) {
            m_parameters->set_dofs(val);
        }

        bool is_thickness_dof(size_t i) const {
            return i < m_parameters->get_num_thickness_dofs();
        }

        void save_dofs(const std::string& filename) const {
            m_parameters->save_dofs(filename);
        }

        void load_dofs(const std::string& filename) {
            m_parameters->load_dofs(filename);
        }

        void periodic_inflate(bool use_reflective_inflator=false);

        /**
         * Return true only if tetgen succeeded.
         */
        bool run_tetgen(Float max_tet_vol=0.0);

        Mesh::Ptr get_mesh() { return m_mesh; }
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        MatrixIr get_voxels() const { return m_voxels; }
        std::vector<MatrixFr> get_shape_velocities() const { return m_shape_velocities; }
        VectorI get_face_sources() const { return m_face_sources; }
        bool is_printable();


        // Debug functions
        WireNetwork::Ptr get_wire_network() { return m_wire_network; }

    private:
        void update_mesh();
        void save_mesh(const std::string& filename) const;

    private:
        Float m_default_thickness;
        WireNetwork::Ptr m_wire_network;
        ParameterManager::Ptr m_parameters;
        WireProfile::Ptr m_profile;

        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_voxels;
        VectorI  m_face_sources;
        Mesh::Ptr m_mesh;
        std::vector<MatrixFr> m_shape_velocities;

        std::string m_refine_algorithm;
        size_t m_refine_order;
};

}
