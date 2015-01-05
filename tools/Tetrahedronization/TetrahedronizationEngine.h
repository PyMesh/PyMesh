#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

class TetrahedronizationEngine {
    public:
        typedef std::shared_ptr<TetrahedronizationEngine> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        TetrahedronizationEngine() :
            m_edge_size(1.0),
            m_face_angle(25),
            m_face_size(1),
            m_face_distance(0.1),
            m_cell_radius_edge_ratio(2),
            m_cell_size(1.5) {}

        virtual ~TetrahedronizationEngine() {}

    public:
        virtual void run()=0;

        void set_vertices(const MatrixFr& vertices) { m_vertices = vertices; }
        void set_faces(const MatrixIr& faces) { m_faces = faces; }

        void set_edge_size(Float val) { m_edge_size = val; }
        void set_face_angle(Float val) { m_face_angle = val; }
        void set_face_size(Float val) { m_face_size = val; }
        void set_face_distance(Float val) { m_face_distance = val; }
        void set_cell_radius_edge_ratio(Float val) { m_cell_radius_edge_ratio = val; }
        void set_cell_size(Float val) { m_cell_size = val; }

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        MatrixIr get_voxels() const { return m_voxels; }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_voxels;

        Float m_edge_size;
        Float m_face_angle;
        Float m_face_size;
        Float m_face_distance;
        Float m_cell_radius_edge_ratio;
        Float m_cell_size;
};
