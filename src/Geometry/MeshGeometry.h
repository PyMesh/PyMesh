/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {

/**
 * MeshGeometry class stores the geometry and geometry only.
 * Explicitly, it keeps an array of vertices, faces and voxels.
 * The public method is left intentionally minimal.
 */
class MeshGeometry {
    public:
        MeshGeometry() : m_dim(3) {}
        virtual ~MeshGeometry() {}

    public:
        VectorF& get_vertices() { return m_vertices; }
        void set_vertices(const VectorF& vertices)  { m_vertices = vertices; }

        VectorI& get_faces() { return m_faces; }
        void set_faces(const VectorI& faces) { m_faces = faces; }

        VectorI& get_voxels() { return m_voxels; }
        void set_voxels(const VectorI& voxels) { m_voxels = voxels; }

        size_t get_vertex_per_face() const { return m_vertex_per_face; }
        void set_vertex_per_face(int v) { m_vertex_per_face = v; }

        size_t get_vertex_per_voxel() const { return m_vertex_per_voxel; }
        void set_vertex_per_voxel(int v) { m_vertex_per_voxel = v; }

        size_t get_dim() const { return m_dim; }
        void set_dim(int v) { m_dim = v; }

        size_t get_num_vertices() const {
            return m_vertices.size() / m_dim;
        }

        size_t get_num_faces() const {
            return m_faces.size() / m_vertex_per_face;
        }

        size_t get_num_voxels() const {
            if (m_vertex_per_voxel != 0)
                return m_voxels.size() / m_vertex_per_voxel;
            else
                return 0;
        }

        void extract_faces_from_voxels();
        int project_out_zero_dim();
    protected:
        void extract_faces_from_tets();
        void extract_faces_from_hexes();

    protected:
        size_t m_dim;
        size_t m_vertex_per_face;
        size_t m_vertex_per_voxel;

        VectorF m_vertices;
        VectorI m_faces;
        VectorI m_voxels;
};

}
