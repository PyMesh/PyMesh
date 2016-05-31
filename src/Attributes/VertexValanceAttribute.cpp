/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexValanceAttribute.h"

#include <set>
#include <sstream>

#include <Mesh.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>

using namespace PyMesh;

void VertexValanceAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();

    if (num_voxels == 0) {
        compute_from_surface_mesh(mesh);
    } else if (num_vertex_per_voxel == 4) {
        compute_from_tet_mesh(mesh);
    } else if (num_vertex_per_voxel == 8) {
        compute_from_hex_mesh(mesh);
    } else {
        std::stringstream err_msg;
        err_msg << "Vertex valance computation is not supported on mesh with "
            << num_vertex_per_voxel << " verices per voxel";
        throw NotImplementedError(err_msg.str());
    }
}

void VertexValanceAttribute::compute_from_surface_mesh(Mesh& mesh) {
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& vertex_valance = m_values;
    vertex_valance = VectorF::Zero(num_vertices);

    std::set<Triplet> edges;
    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        for (size_t j=0; j<num_vertex_per_face; j++) {
            Triplet edge(face[j], face[(j+1)%num_vertex_per_face]);
            edges.insert(edge);
        }
    }

    for (auto edge : edges) {
        const Vector3I& data = edge.get_ori_data();
        vertex_valance[data[0]] ++;
        vertex_valance[data[1]] ++;
    }
}

void VertexValanceAttribute::compute_from_tet_mesh(Mesh& mesh) {
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();
    assert(num_vertex_per_voxel == 4);

    VectorF& vertex_valance = m_values;
    vertex_valance = VectorF::Zero(num_vertices);

    std::set<Triplet> edges;
    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh.get_voxel(i);
        Triplet edge[6] = {
            Triplet(voxel[0], voxel[1]),
            Triplet(voxel[0], voxel[2]),
            Triplet(voxel[0], voxel[3]),
            Triplet(voxel[1], voxel[2]),
            Triplet(voxel[1], voxel[3]),
            Triplet(voxel[2], voxel[3]) };
        edges.insert(edge[0]);
        edges.insert(edge[1]);
        edges.insert(edge[2]);
        edges.insert(edge[3]);
        edges.insert(edge[4]);
        edges.insert(edge[5]);
    }

    for (auto edge : edges) {
        const Vector3I& data = edge.get_ori_data();
        vertex_valance[data[0]] ++;
        vertex_valance[data[1]] ++;
    }
}

void VertexValanceAttribute::compute_from_hex_mesh(Mesh& mesh) {
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t num_vertex_per_voxel = mesh.get_vertex_per_voxel();
    assert(num_vertex_per_voxel == 8);
    // Vertex ordering
    //  3 _________ 2
    //   /:       /|
    // 7/ :      / |
    // +--------+6 |
    // |  :     |  |
    // | 0:.....|..|1
    // | ;      | /
    // +--------+/
    // 4         5

    VectorF& vertex_valance = m_values;
    vertex_valance = VectorF::Zero(num_vertices);

    std::set<Triplet> edges;
    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh.get_voxel(i);
        Triplet edge[12] = {
            Triplet(voxel[0], voxel[1]),
            Triplet(voxel[1], voxel[2]),
            Triplet(voxel[2], voxel[3]),
            Triplet(voxel[3], voxel[0]),
            Triplet(voxel[4], voxel[5]),
            Triplet(voxel[5], voxel[6]),
            Triplet(voxel[6], voxel[7]),
            Triplet(voxel[7], voxel[4]),
            Triplet(voxel[0], voxel[4]),
            Triplet(voxel[1], voxel[5]),
            Triplet(voxel[2], voxel[6]),
            Triplet(voxel[3], voxel[7]) };

        edges.insert(edge[0]);
        edges.insert(edge[1]);
        edges.insert(edge[2]);
        edges.insert(edge[3]);
        edges.insert(edge[4]);
        edges.insert(edge[5]);
        edges.insert(edge[6]);
        edges.insert(edge[7]);
        edges.insert(edge[8]);
        edges.insert(edge[9]);
        edges.insert(edge[10]);
        edges.insert(edge[11]);
    }

    for (auto edge : edges) {
        const Vector3I& data = edge.get_ori_data();
        vertex_valance[data[0]] ++;
        vertex_valance[data[1]] ++;
    }
}
