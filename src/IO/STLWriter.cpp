#include "STLWriter.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

void STLWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .stl format does not support attributes." << std::endl;
}

void STLWriter::write_mesh(Mesh& mesh) {
    write(mesh.get_vertices(),
            mesh.get_faces(),
            mesh.get_voxels(),
            mesh.get_dim(),
            mesh.get_vertex_per_face(),
            mesh.get_vertex_per_voxel());
}

void STLWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    if (m_in_ascii) {
        write_ascii(vertices, faces, voxels,
                dim, vertex_per_face, vertex_per_voxel);
    } else {
        write_binary(vertices, faces, voxels,
                dim, vertex_per_face, vertex_per_voxel);
    }
}

void STLWriter::check_mesh(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) const {
    if (vertex_per_face == 0) {
        if (faces.size() != 0) {
            throw IOError("Unsupported face type with 0 vertex per face.");
        }
        // Since there is 0 faces, assume they are triangles.
        vertex_per_face = 3;
    } else if (vertex_per_face != 3) {
        std::stringstream err_msg;
        err_msg << "STL format does not support non-triangle face with "
            << vertex_per_face << " vertices." << std::endl;
        throw IOError(err_msg.str());
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
}

void STLWriter::write_ascii(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    check_mesh(vertices, faces, voxels, dim, vertex_per_face, vertex_per_voxel);
    std::ofstream fout(m_filename.c_str());
    fout.precision(16);
    if (is_anonymous()) {
        fout << "solid" << std::endl;
    } else {
        fout << "solid generated with PyMesh" << std::endl;
    }

    auto get_vertex = [&](size_t i)->Vector3F {
        Vector3F v(0.0, 0.0, 0.0);
        v.segment(0, dim) = vertices.segment(i*dim, dim);
        return v;
    };

    const size_t num_faces = faces.size() / vertex_per_face;
    for (size_t i=0; i<num_faces; i++) {
        Vector3I f = faces.segment<3>(i*3);
        Vector3F v0 = get_vertex(f[0]);
        Vector3F v1 = get_vertex(f[1]);
        Vector3F v2 = get_vertex(f[2]);
        fout << "facet normal 0 0 0" << std::endl;
        fout << "outer loop" << std::endl;
        fout << "vertex " << v0[0] << " " << v0[1] << " " << v0[2] << std::endl;
        fout << "vertex " << v1[0] << " " << v1[1] << " " << v1[2] << std::endl;
        fout << "vertex " << v2[0] << " " << v2[1] << " " << v2[2] << std::endl;
        fout << "endloop" << std::endl;
        fout << "endfacet" << std::endl;
    }

    fout << "endsolid generated with PyMesh" << std::endl;
    fout.close();
}

void STLWriter::write_binary(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    check_mesh(vertices, faces, voxels, dim, vertex_per_face, vertex_per_voxel);
    std::ofstream fout(m_filename.c_str());

    char header[80];
    if (!is_anonymous()) {
        strcpy(header, "Generated with PyMesh");
    }
    fout.write(header, 80);

    auto get_vertex = [&](size_t i)->Vector3F {
        Vector3F v(0.0, 0.0, 0.0);
        v.segment(0, dim) = vertices.segment(i*dim, dim);
        return v;
    };

    auto write_vertex = [&](size_t vid) {
        Eigen::Vector3f v = get_vertex(vid).cast<float>();
        fout.write((char*)&v[0], sizeof(float));
        fout.write((char*)&v[1], sizeof(float));
        fout.write((char*)&v[2], sizeof(float));
    };

    const float zero_f = 0.0;
    const uint16_t zero_16 = 0;
    uint32_t num_faces = faces.rows() / vertex_per_face;
    fout.write((char*)&num_faces, sizeof(uint32_t));
    for (size_t i=0; i<num_faces; i++) {
        // Normal vector.
        fout.write((char*)&zero_f, sizeof(float));
        fout.write((char*)&zero_f, sizeof(float));
        fout.write((char*)&zero_f, sizeof(float));

        Vector3I f = faces.segment<3>(i*3);
        write_vertex(f[0]);
        write_vertex(f[1]);
        write_vertex(f[2]);

        fout.write((char*)&zero_16, sizeof(uint16_t));
    }
}

