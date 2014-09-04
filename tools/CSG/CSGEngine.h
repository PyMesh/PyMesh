#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

class CSGEngine {
    public:
        typedef std::shared_ptr<CSGEngine> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        virtual ~CSGEngine() {}

    public:
        void set_mesh_1(const MatrixFr& vertices, const MatrixIr& faces) {
            m_vertices_1 = vertices;
            m_faces_1 = faces;
        }

        void set_mesh_2(const MatrixFr& vertices, const MatrixIr& faces) {
            m_vertices_2 = vertices;
            m_faces_2 = faces;
        }

        const MatrixFr& get_vertices() const { return m_vertices; }
        const MatrixIr& get_faces() const { return m_faces; }
        void clean_up();

    public:
        virtual void compute_union() =0;
        virtual void compute_intersection() =0;
        virtual void compute_difference() =0;
        virtual void compute_symmetric_difference() =0;

    protected:
        void remove_duplicated_vertices();
        void remove_short_edges();
        void remove_isolated_vertices();

    protected:
        MatrixFr m_vertices_1;
        MatrixIr m_faces_1;

        MatrixFr m_vertices_2;
        MatrixIr m_faces_2;

        MatrixFr m_vertices;
        MatrixIr m_faces;
};
