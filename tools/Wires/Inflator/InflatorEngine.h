#pragma once

#include <memory>
#include <string>

#include <Wires/WireNetwork/WireNetwork.h>
#include "WireProfile.h"

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

    public:
        InflatorEngine(WireNetwork::Ptr wire_network);
        virtual ~InflatorEngine() {}

    public:
        virtual void inflate()=0;

    public:
        void refine(const std::string& algorithm, size_t order);
        VectorF  get_thickness() const { return m_thickness; }
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_sources() const { return m_face_sources; }
        ThicknessType get_thickness_type() const { return m_thickness_type; }

        void set_thickness(const VectorF& thickness);
        void set_thickness_type(ThicknessType type) { m_thickness_type = type; }
        void set_profile(WireProfilePtr profile) { m_profile = profile; }

    protected:
        void clean_up();
        void remove_isolated_vertices();
        void remove_duplicated_vertices();
        void remove_short_edges();
        void save_mesh(const std::string& filename, const MatrixFr& vertices,
                const MatrixIr& faces, VectorF debug);
        void check_thickness() const;

    protected:
        WireNetwork::Ptr m_wire_network;

        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorF m_thickness;
        ThicknessType m_thickness_type;
        WireProfilePtr m_profile;

        /**
         * m_face_sources tracks the source element of each inflated face.
         * I.e. which vertex or edge of the wire network that generates this
         * face.  Vertex index is stored as positive values, starting from 1.
         * Edge index is stored as negative values, starting from -1.
         */
        VectorI m_face_sources;
};
