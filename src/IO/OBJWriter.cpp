/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OBJWriter.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

void OBJWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .obj format does not support attributes." << std::endl;
}

void OBJWriter::write_mesh(Mesh& mesh) {
    write(mesh.get_vertices(),
            mesh.get_faces(),
            mesh.get_voxels(),
            mesh.get_dim(),
            mesh.get_vertex_per_face(),
            mesh.get_vertex_per_voxel());
}

void OBJWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    if (vertex_per_face != 3 && vertex_per_face != 4) {
        std::stringstream err_msg;
        err_msg << "OBJ format does not support non-triangle non-quad face with "
            << vertex_per_face << " vertices." << std::endl;
        throw RuntimeError(err_msg.str());
    }

    std::ofstream fout(m_filename.c_str());
    fout.precision(16);
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }

    size_t num_vertices = vertices.size() / dim;
    for (size_t i=0; i<num_vertices; i++) {
        const auto& v = vertices.segment(i*dim, dim);
        fout << "v ";
        for (size_t j=0; j<dim; j++) {
            fout << v[j] << " ";
        }
        fout << std::endl;
    }

    size_t num_faces = faces.size() / vertex_per_face;
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = faces.segment(i*vertex_per_face, vertex_per_face);
        fout << "f ";
        for (size_t j=0; j<vertex_per_face; j++) {
            fout << f[j] + 1 << " ";
        }
        fout << std::endl;
    }
    fout.close();
}
