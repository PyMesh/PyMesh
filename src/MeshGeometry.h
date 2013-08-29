#pragma once

#include <string>
#include "EigenTypedef.h"

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

        VectorI& get_faces() { return m_faces; }

        VectorI& get_voxels() { return m_voxels; }

        int get_vertex_per_face() const { return m_vertex_per_face; }
        void set_vertex_per_face(int v) { m_vertex_per_face = v; }

        int get_vertex_per_voxel() const { return m_vertex_per_voxel; }
        void set_vertex_per_voxel(int v) { m_vertex_per_voxel = v; }

    protected:
        int m_dim;
        int m_vertex_per_face;
        int m_vertex_per_voxel;

        VectorF m_vertices;
        VectorI m_faces;
        VectorI m_voxels;
};

