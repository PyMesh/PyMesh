/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

namespace PyMesh {

class Mesh;

/**
 * Limitation: only support triangles and tetrahedrons.
 */
class MeshConnectivity {
    public:
        void initialize(Mesh* mesh);

        VectorI get_vertex_adjacent_vertices(size_t vi) const;
        VectorI get_vertex_adjacent_faces(size_t vi) const;
        VectorI get_vertex_adjacent_voxels(size_t vi) const;

        VectorI get_face_adjacent_faces(size_t fi) const;
        VectorI get_face_adjacent_voxels(size_t fi) const;

        VectorI get_voxel_adjacent_faces(size_t Vi) const;
        VectorI get_voxel_adjacent_voxels(size_t Vi) const;

    public:
        bool vertex_adjacencies_computed() const;
        bool face_adjacencies_computed() const;
        bool voxel_adjacencies_computed() const;

        void init_vertex_adjacencies(Mesh* mesh);
        void init_face_adjacencies(Mesh* mesh);
        void init_voxel_adjacencies(Mesh* mesh);

        void clear();

    protected:
        VectorI m_vertex_adjacency;
        VectorI m_vertex_adjacency_idx;

        VectorI m_vertex_face_adjacency;
        VectorI m_vertex_face_adjacency_idx;

        VectorI m_vertex_voxel_adjacency;
        VectorI m_vertex_voxel_adjacency_idx;

        VectorI m_face_adjacency;
        VectorI m_face_adjacency_idx;

        VectorI m_face_voxel_adjacency;
        VectorI m_face_voxel_adjacency_idx;

        VectorI m_voxel_adjacency;
        VectorI m_voxel_adjacency_idx;

        VectorI m_voxel_face_adjacency;
        VectorI m_voxel_face_adjacency_idx;
};

}
