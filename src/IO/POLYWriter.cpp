#include "POLYWriter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <Core/Exception.h>

using namespace PyMesh;

void POLYWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .poly format does not support attributes ... yet." <<
        std::endl;
}

void POLYWriter::write_mesh(Mesh& mesh) {
    write(mesh.get_vertices(),
            mesh.get_faces(),
            mesh.get_voxels(),
            mesh.get_dim(),
            mesh.get_vertex_per_face(),
            mesh.get_vertex_per_voxel());
}

void POLYWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    if (dim != 3) {
        throw IOError(".poly format only supports 3D meshes.");
    }
    if (vertices.size() % dim != 0) {
        throw IOError("Invalid vertex array.");
    }
    if (faces.size() % vertex_per_face != 0) {
        throw IOError("Invalid face array.");
    }

    const size_t num_vertices = vertices.size() / dim;
    const size_t num_faces = faces.size() / vertex_per_face;

    std::ofstream fout(m_filename.c_str());
    if (!fout.good()) {
        std::stringstream err_msg;
        err_msg << "Cannot open file " << m_filename;
        throw IOError(err_msg.str());
    }
    fout.precision(16);
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }

    // Node list
    fout << num_vertices
         << " 3 0 0" << std::endl; // 3 dim, 0 attributes, 0 bd markers.
    for (size_t i=0; i<num_vertices; i++) {
        fout << i << " "
             << vertices[i*3  ] << " "
             << vertices[i*3+1] << " "
             << vertices[i*3+2] << std::endl;
    }

    // Face list
    fout << num_faces << " 0" << std::endl;
    for (size_t i=0; i<num_faces; i++) {
        fout << "1 0 0" << std::endl; // 1 polygon, 0 holes, 0 bd markers.
        fout << vertex_per_face;
        for (size_t j=0; j<vertex_per_face; j++) {
            fout << " " << faces[i*vertex_per_face + j];
        }
        fout << std::endl;
    }
    fout.close();

    // Hole list
    fout << 0 << std::endl;

    // Attribute list
    fout << 0 << std::endl;
}
