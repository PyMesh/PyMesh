#include "NodeWriter.h"
#include "IOUtils.h"

#include <iostream>
#include <fstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>

using namespace PyMesh;

void NodeWriter::with_attribute(const std::string& attr_name) {
    if (attr_name == "region") {
        m_with_region_attribute = true;
    } else if (attr_name == "node_boundary_marker") {
        m_with_node_bd_marker = true;
    } else if (attr_name == "face_boundary_marker") {
        m_with_face_bd_marker = true;
    } else {
        std::cerr << "Error: .node/.face/.ele formats does not support named "
            "attributes" << std::endl;
    }
}

void NodeWriter::write_mesh(Mesh& mesh) {
    const std::string name = IOUtils::get_name(m_filename);
    const std::string node_filename = name + ".node";
    const std::string face_filename = name + ".face";
    const std::string elem_filename = name + ".ele";

    write_node_file(node_filename, mesh);
    write_face_file(face_filename, mesh);
    write_elem_file(elem_filename, mesh);
}

void NodeWriter::write(
        const VectorF& vertices,
        const VectorI& faces,
        const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    Mesh::Ptr mesh = MeshFactory()
        .load_data(vertices, faces, voxels, dim, vertex_per_face, vertex_per_voxel)
        .create_shared();
    write_mesh(*mesh);
}

void NodeWriter::write_node_file(const std::string& filename, Mesh& mesh) {
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t dim = mesh.get_dim();
    std::ofstream fout(filename.c_str());
    fout.precision(16);
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }
    VectorF bd_marker;
    if (m_with_node_bd_marker) {
        if (!mesh.has_attribute("node_boundary_marker")) {
            throw IOError("Attribute \"node_boundary_marker\" does not exist.");
        } else {
            bd_marker = mesh.get_attribute("node_boundary_marker");
            assert(num_vertices == bd_marker.size());
        }
    }
    const VectorF& vertices = mesh.get_vertices();
    fout << num_vertices << " " << dim << " 0 " << m_with_node_bd_marker
        << std::endl;
    for (size_t i=0; i<num_vertices; i++) {
        fout << i;
        for (size_t j=0; j<dim; j++) {
            fout << " " << vertices[i*dim + j];
        }
        if (m_with_node_bd_marker) {
            fout << " " << bd_marker[i];
        }
        fout << std::endl;
    }
}

void NodeWriter::write_face_file(const std::string& filename, Mesh& mesh) {
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();
    if (num_faces == 0) return;
    if (vertex_per_face != 3) {
        throw IOError("Only triangle is supported in .face file.");
    }

    std::ofstream fout(filename.c_str());
    fout.precision(16);
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }
    VectorF bd_marker;
    if (m_with_face_bd_marker) {
        if (!mesh.has_attribute("face_boundary_marker")) {
            throw IOError("Attribute \"face_boundary_marker\" does not exist.");
        } else {
            bd_marker = mesh.get_attribute("face_boundary_marker");
            assert(num_faces == bd_marker.size());
        }
    }

    const VectorI& faces = mesh.get_faces();
    fout << num_faces << " " << m_with_face_bd_marker << std::endl;
    for (size_t i=0; i<num_faces; i++) {
        fout << i;
        for (size_t j=0; j<vertex_per_face; j++) {
            fout << " " << faces[i*vertex_per_face+ j];
        }
        if (m_with_face_bd_marker) {
            fout << " " << bd_marker[i];
        }
        fout << std::endl;
    }
}

void NodeWriter::write_elem_file(const std::string& filename, Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    if (num_voxels == 0) return;
    if (vertex_per_voxel != 4) {
        throw IOError("Only tet element is supported in .ele file.");
    }

    std::ofstream fout(filename.c_str());
    fout.precision(16);
    if (!is_anonymous()) {
        fout << "# Generated with PyMesh" << std::endl;
    }
    VectorF region;
    if (m_with_region_attribute) {
        if (!mesh.has_attribute("region")) {
            throw IOError("Attribute \"region\" does not exist.");
        } else {
            region = mesh.get_attribute("region");
            assert(num_voxels == region.size());
        }
    }

    const VectorI& voxels = mesh.get_voxels();
    fout << num_voxels << " 4 " << m_with_region_attribute << std::endl;
    for (size_t i=0; i<num_voxels; i++) {
        fout << i;
        for (size_t j=0; j<4; j++) {
            fout << " " << voxels[i*4+ j];
        }
        if (m_with_region_attribute) {
            fout << " " << region[i];
        }
        fout << std::endl;
    }
}

