#include "OFFWriter.h"

#include <iostream>
#include <fstream>

#include <Core/Exception.h>

using namespace PyMesh;

void OFFWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .off format does not support attributes." << std::endl;
}

void OFFWriter::write_mesh(Mesh& mesh) {
    write(mesh.get_vertices(),
            mesh.get_faces(),
            mesh.get_voxels(),
            mesh.get_dim(),
            mesh.get_vertex_per_face(),
            mesh.get_vertex_per_voxel());
}

void OFFWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    if (vertex_per_face == 0) {
        if (faces.size() != 0) {
            throw IOError("Unsupported face type with 0 vertex per face.");
        }
        // Since there is 0 faces, assume they are triangles.
        vertex_per_face = 3;
    } else if (vertex_per_face != 3 && vertex_per_face != 4) {
        std::cerr << "Error: non-triangle non-quad face with "
            << vertex_per_face << " vertices." << std::endl;
        throw RuntimeError("Unsupported face type.");
    }

    if (dim != 3 && dim != 2) {
        throw IOError("Only 2D and 3D meshes are supported.");
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
    fout.precision(16);
    fout << "OFF" << std::endl;
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }
    fout << num_vertices << " " << num_faces << " 0" << std::endl;
    if (dim == 3) {
        for (size_t i=0; i<num_vertices; i++) {
            for (size_t j=0; j<dim; j++) {
                fout << vertices[i*dim+j] << " ";
            }
            fout << std::endl;
        }
    } else if (dim == 2) {
        for (size_t i=0; i<num_vertices; i++) {
            for (size_t j=0; j<dim; j++) {
                fout << vertices[i*dim+j] << " ";
            }
            fout << 0 << std::endl;
        }
    } else {
        throw IOError("Only 2D and 3D meshes are supported.");
    }

    for (size_t i=0; i<num_faces; i++) {
        fout << vertex_per_face << " ";
        for (size_t j=0; j<vertex_per_face; j++) {
            fout << faces[i*vertex_per_face+j] << " ";
        }
        fout << std::endl;
    }
    fout.close();
}
