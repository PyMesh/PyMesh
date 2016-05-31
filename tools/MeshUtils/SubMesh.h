/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <Mesh.h>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class SubMesh {
    public:
        typedef std::shared_ptr<SubMesh> Ptr;
        static Ptr create(Mesh::Ptr mesh);
        static Ptr create_raw(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        SubMesh(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        virtual void filter_with_vertex_index(
                const std::vector<size_t>& selected_vertex_indices);
        virtual void filter_with_face_index(
                const std::vector<size_t>& selected_face_indices);

        /**
         * Only vertices that evaluate to true are kept.
         */
        virtual void filter_vertex_with_custom_function(
                const std::function<bool(const VectorF&)>& func);

        /**
         * Union all selected vertices and faces.
         * A face is selected iff all of its vertices are selected.
         */
        void finalize();

        // Selected geometry
        MatrixFr get_selected_vertices() const {
            check_validity(); return m_selected_vertices;
        }
        MatrixIr get_selected_faces() const {
            check_validity(); return m_selected_faces;
        }

        VectorI get_selected_vertex_indices() const {
            check_validity(); return m_selected_vertex_indices;
        }
        VectorI get_selected_face_indices() const {
            check_validity(); return m_selected_face_indices;
        }

        // Unselected geometry
        MatrixFr get_unselected_vertices() const {
            check_validity(); return m_unselected_vertices;
        }
        MatrixIr get_unselected_faces() const {
            check_validity(); return m_unselected_faces;
        }

        VectorI get_unselected_vertex_indices() const {
            check_validity(); return m_unselected_vertex_indices;
        }
        VectorI get_unselected_face_indices() const {
            check_validity(); return m_unselected_face_indices;
        }

    protected:
        void collect_selected_faces();
        void clean_up_selected();
        void clean_up_unselected();
        void check_validity() const;

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;

        MatrixFr m_selected_vertices;
        MatrixIr m_selected_faces;
        VectorI m_selected_vertex_indices;
        VectorI m_selected_face_indices;

        MatrixFr m_unselected_vertices;
        MatrixIr m_unselected_faces;
        VectorI m_unselected_vertex_indices;
        VectorI m_unselected_face_indices;

        std::vector<bool> m_vertex_selection;
};

}
