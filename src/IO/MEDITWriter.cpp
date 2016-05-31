/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MEDITWriter.h"
#include <Core/Exception.h>
#include <iostream>

using namespace PyMesh;

void MEDITWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .mesh format does not support attributes." << std::endl;
}

void MEDITWriter::write_mesh(Mesh& mesh) {
    write(mesh.get_vertices(),
            mesh.get_faces(),
            mesh.get_voxels(),
            mesh.get_dim(),
            mesh.get_vertex_per_face(),
            mesh.get_vertex_per_voxel());
}

void MEDITWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    std::ofstream fout(m_filename.c_str());
    fout.precision(16);
    fout << "MeshVersionFormatted 1" << std::endl;
    fout << "Dimension 3" << std::endl;
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }

    write_vertices(fout, vertices, dim);
    write_faces(fout, faces, vertex_per_face);
    write_voxels(fout, voxels, vertex_per_voxel);

    fout.close();
}

void MEDITWriter::write_vertices(
        std::ofstream& fout, const VectorF& vertices, const size_t dim) {
    fout << "Vertices" << std::endl;
    const size_t num_vertices = vertices.size() / dim;
    fout << num_vertices << std::endl;
    if (dim == 3) {
        for (size_t i=0; i<num_vertices; i++) {
            fout << vertices[i*dim  ] << " ";
            fout << vertices[i*dim+1] << " ";
            fout << vertices[i*dim+2] << " ";
            fout << -1 << std::endl;
        }
    } else if (dim == 2) {
        for (size_t i=0; i<num_vertices; i++) {
            fout << vertices[i*dim  ] << " ";
            fout << vertices[i*dim+1] << " ";
            fout << 0.0 << " ";
            fout << -1 << std::endl;
        }
    } else {
        throw IOError("Only 2D and 3D mesh are supported.");
    }
}

void MEDITWriter::write_faces(
        std::ofstream& fout, const VectorI& faces,
        const size_t vertex_per_face) {
    if (faces.size() == 0) return;
    if (vertex_per_face == 3) {
        fout << "Triangles" << std::endl;
    } else if (vertex_per_face == 4) {
        fout << "Quadrilaterals" << std::endl;
    } else {
        throw IOError("Only triangle and quad faces are supported.");
    }
    const size_t num_faces = faces.size() / vertex_per_face;
    assert(faces.size() % vertex_per_face == 0);
    fout << num_faces << std::endl;
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            fout << faces[i*vertex_per_face+j]+1 << " ";
        }
        fout << -1 << std::endl;
    }
}

void MEDITWriter::write_voxels(
        std::ofstream& fout, const VectorI& voxels,
        const size_t vertex_per_voxel) {
    if (voxels.size() == 0) return;
    if (vertex_per_voxel == 4) {
        fout << "Tetrahedra" << std::endl;
    } else if (vertex_per_voxel == 8) {
        fout << "Hexahedra" << std::endl;
    } else {
        throw IOError("Only tet and hex voxels are supported.");
    }
    const size_t num_voxels = voxels.size() / vertex_per_voxel;
    assert(voxels.size() % vertex_per_voxel == 0);
    fout << num_voxels << std::endl;
    for (size_t i=0; i<num_voxels; i++) {
        for (size_t j=0; j<vertex_per_voxel; j++) {
            fout << voxels[i*vertex_per_voxel+j]+1 << " ";
        }
        fout << -1 << std::endl;
    }
}

