#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <Mesh.h>
#include <Core/EigenTypedef.h>

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
         * Union all selected vertices, faces and voxels.
         */
        void finalize();

        const MatrixFr& get_vertices() const { check_validity(); return m_vertices; }
        const MatrixIr& get_faces() const { check_validity(); return m_faces; }

        const VectorI& get_ori_vertex_indices() const { check_validity(); return m_ori_vertex_indices; }
        const VectorI& get_ori_face_indices() const { check_validity(); return m_ori_face_indices; }

        void check_validity() const;

    protected:
        void collect_selected_vertices();
        void collect_selected_faces();
        void remove_isolated_vertices();

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;

        VectorI m_ori_vertex_indices;
        VectorI m_ori_face_indices;

        std::vector<bool> m_vertex_selection;
};
