/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OBJWriter.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

namespace OBJWriterHelper {
    void write_vertices(std::ofstream& fout,
            const VectorF& vertices, const size_t dim) {
        if (dim != 2 && dim != 3) {
            throw IOError("Unsupported mesh dimension: " + std::to_string(dim));
        }
        fout.precision(16);
        size_t num_vertices = vertices.size() / dim;
        for (size_t i=0; i<num_vertices; i++) {
            const auto& v = vertices.segment(i*dim, dim);
            fout << "v";
            for (size_t j=0; j<dim; j++) {
                fout << " " << v[j];
            }
            fout << std::endl;
        }
    }

    void write_texture(std::ofstream& fout, const VectorF& uv) {
        assert(uv.size() % 2 == 0);
        const size_t num_uvs = uv.size() / 2;
        for (size_t i=0; i<num_uvs; i++) {
            fout << "vt " << uv[i*2] << " " << uv[i*2+1] << std::endl;
        }
    }

    void write_faces(std::ofstream& fout,
            const VectorI& faces,
            const size_t vertex_per_face,
            const VectorI& uv_indices=VectorI::Zero(0)) {
        if (vertex_per_face != 3 && vertex_per_face != 4) {
            std::stringstream err_msg;
            err_msg << "OBJ format does not support non-triangle non-quad face with "
                << vertex_per_face << " vertices." << std::endl;
            throw RuntimeError(err_msg.str());
        }
        const size_t num_faces = faces.size() / vertex_per_face;
        const size_t num_uvs = uv_indices.size() / 2;
        if (num_uvs == 0) {
            for (size_t i=0; i<num_faces; i++) {
                const auto& f = faces.segment(i*vertex_per_face, vertex_per_face);
                fout << "f";
                for (size_t j=0; j<vertex_per_face; j++) {
                    fout << " " << f[j] + 1;
                }
                fout << std::endl;
            }
        } else {
            for (size_t i=0; i<num_faces; i++) {
                const auto& f = faces.segment(i*vertex_per_face, vertex_per_face);
                const auto& uv = uv_indices.segment(i*vertex_per_face, vertex_per_face);
                fout << "f ";
                for (size_t j=0; j<vertex_per_face; j++) {
                    fout << f[j] + 1 << "/" << uv[j] + 1 << " ";
                }
                fout << std::endl;
            }
        }
    }
}

void OBJWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .obj format does not support attributes." << std::endl;
}

void OBJWriter::write_mesh(Mesh& mesh) {
    using namespace OBJWriterHelper;
    std::ofstream fout(m_filename.c_str());
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }
    VectorF texture;
    VectorI texture_indices;
    if (mesh.has_attribute("corner_texture")) {
        texture = mesh.get_attribute("corner_texture");
        const size_t num_faces = mesh.get_num_faces();
        const size_t vertex_per_face = mesh.get_vertex_per_face();
        if (texture.size() != num_faces * vertex_per_face * 2) {
            // Texture invalid.
            texture.resize(0);
        } else {
            write_texture(fout, texture);
            texture_indices.resize(num_faces * vertex_per_face);
            for (size_t i=0; i<num_faces; i++) {
                for (size_t j=0; j<vertex_per_face; j++) {
                    texture_indices[i*vertex_per_face+j] = i*vertex_per_face+j;
                }
            }
        }
    }

    write_vertices(fout, mesh.get_vertices(), mesh.get_dim());
    write_faces(fout, mesh.get_faces(), mesh.get_vertex_per_face(),
            texture_indices);
    fout.close();
}

void OBJWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    using namespace OBJWriterHelper;
    std::ofstream fout(m_filename.c_str());
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }

    write_vertices(fout, vertices, dim);
    write_faces(fout, faces, vertex_per_face);
    fout.close();
}
