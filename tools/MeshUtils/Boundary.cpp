/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Boundary.h"

#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include "BoundaryEdges.h"
#include "BoundaryFaces.h"

using namespace PyMesh;

Boundary::Ptr Boundary::extract_surface_boundary(const Mesh& mesh) {
    return Ptr(new BoundaryEdges(mesh));
}

Boundary::Ptr Boundary::extract_surface_boundary_raw(
        MatrixFr& vertices, MatrixIr& faces) {
    VectorF flattened_vertices = Eigen::Map<VectorF>(vertices.data(),
            vertices.rows() * vertices.cols());
    VectorI flattened_faces = Eigen::Map<VectorI>(faces.data(),
            faces.rows() * faces.cols());
    VectorI voxels = VectorI::Zero(0);

    MeshFactory factory;
    Mesh::Ptr mesh = factory.load_data(flattened_vertices, flattened_faces,
            voxels, vertices.cols(), faces.cols(), 0).create_shared();

    return extract_surface_boundary(*mesh);
}

Boundary::Ptr Boundary::extract_volume_boundary(const Mesh& mesh) {
    if (mesh.get_dim() == 2) {
        throw RuntimeError("2D mesh has no volume boundary!");
    }
    if (mesh.get_num_voxels() == 0) {
        throw RuntimeError("Mesh contains 0 voxels");
    }
    return Ptr(new BoundaryFaces(mesh));
}

Boundary::Ptr Boundary::extract_volume_boundary_raw(
        MatrixFr& vertices, MatrixIr& voxels) {
    VectorF flattened_vertices = Eigen::Map<VectorF>(vertices.data(),
            vertices.rows() * vertices.cols());
    VectorI faces = VectorI::Zero(0);
    VectorI flattened_voxels = Eigen::Map<VectorI>(voxels.data(),
            voxels.rows() * voxels.cols());

    size_t vertex_per_voxel = voxels.cols();
    size_t vertex_per_face=0;
    if (vertex_per_voxel == 4) vertex_per_face = 3;
    else if (vertex_per_voxel == 8) vertex_per_face = 4;
    else {
        throw RuntimeError("Unknown voxel type.");
    }

    MeshFactory factory;
    Mesh::Ptr mesh = factory.load_data(flattened_vertices, faces,
            flattened_voxels, vertices.cols(), vertex_per_face,
            vertex_per_voxel).create_shared();

    return extract_volume_boundary(*mesh);
}
