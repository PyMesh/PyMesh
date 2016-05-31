/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <MeshUtils/Subdivision.h>
#include <Wires/WireNetwork/WireNetwork.h>
#include "WireProfile.h"
#include <Wires/Parameters/ParameterManager.h>

namespace PyMesh {

class InflatorEngine {
    public:
        typedef std::shared_ptr<InflatorEngine> Ptr;
        typedef WireProfile::Ptr WireProfilePtr;
        enum ThicknessType {
            PER_VERTEX,
            PER_EDGE
        };

    public:
        static Ptr create(const std::string& type, WireNetwork::Ptr network);
        static Ptr create_parametric(WireNetwork::Ptr network,
                ParameterManager::Ptr manager);
        static Ptr create_isotropic_parametric(WireNetwork::Ptr network,
                ParameterManager::Ptr manager);

    public:
        InflatorEngine(WireNetwork::Ptr wire_network);
        virtual ~InflatorEngine() {}

    public:
        virtual void with_shape_velocities();
        virtual void inflate()=0;
        virtual const std::vector<MatrixFr>& get_shape_velocities() const=0;
        virtual void set_uniform_thickness(Float thickness);
        virtual void set_thickness(const VectorF& thickness);
        virtual void set_thickness_type(ThicknessType type) { m_thickness_type = type; }

    public:
        void self_intersection_is_fatal() { m_self_intersection_is_fatal = true; }
        void with_refinement(const std::string& algorithm, size_t order);
        void with_geometry_correction_lookup(const std::string& csv_file);
        void with_rel_geometry_correction(const VectorF& correction);
        void with_abs_geometry_correction(const VectorF& correction);
        void set_geometry_correction_cap(Float cap) { m_correction_cap = cap; }
        void set_geometry_spread_constant(Float val) { m_spread_const = val; }
        VectorF  get_thickness() const { return m_thickness; }
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_sources() const { return m_face_sources; }
        ThicknessType get_thickness_type() const { return m_thickness_type; }

        void set_profile(WireProfilePtr profile) { m_profile = profile; }

    protected:
        void clean_up();
        void remove_isolated_vertices();
        void remove_duplicated_vertices();
        void remove_short_edges();
        void save_mesh(const std::string& filename, const MatrixFr& vertices,
                const MatrixIr& faces, VectorF debug);
        void save_mesh(const std::string& filename, const MatrixFr& vertices,
                const MatrixIr& faces);
        void check_thickness() const;

    protected:
        WireNetwork::Ptr m_wire_network;

        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorF m_thickness;
        ThicknessType m_thickness_type;
        WireProfilePtr m_profile;
        Subdivision::Ptr m_refiner;
        size_t m_subdiv_order;
        VectorF m_rel_correction;
        VectorF m_abs_correction;
        Float m_correction_cap;
        Float m_spread_const;
        bool m_self_intersection_is_fatal;

        /**
         * m_face_sources tracks the source element of each inflated face.
         * I.e. which vertex or edge of the wire network that generates this
         * face.  Vertex index is stored as positive values, starting from 1.
         * Edge index is stored as negative values, starting from -1.
         */
        VectorI m_face_sources;
};

}
