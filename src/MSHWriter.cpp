#include "MSHWriter.h"

#include <iostream>
#include <fstream>

#include "Mesh.h"
#include "MshSaver.h"

MeshWriter& MSHWriter::with_attribute(const std::string& attr_name) {
    m_attr_names.push_back(attr_name);
    return *this;
}

void MSHWriter::write_mesh(Mesh& mesh) {
    if (mesh.get_num_voxels() == 0) {
        write_surface_mesh(mesh);
    } else {
        write_volume_mesh(mesh);
    }
}

void MSHWriter::write(VectorF& vertices, VectorI& faces, VectorI& voxels,
        size_t vertex_per_face, size_t vertex_per_voxel) {
    Zhou::MshSaver saver(m_filename, true);
    if (voxels.size() == 0) {
        write_geometry(saver, vertices, faces, vertex_per_face);
    } else {
        write_geometry(saver, vertices, voxels, vertex_per_voxel);
    }
    if (m_attr_names.size() != 0) {
        std::cerr << "Warning: all attributes are ignored." << std::endl;
    }
}


void MSHWriter::write_surface_mesh(Mesh& mesh) {
    Zhou::MshSaver saver(m_filename, true);

    size_t dim = mesh.get_dim();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_faces = mesh.get_num_faces();
    size_t vertex_per_face = mesh.get_vertex_per_face();

    write_geometry(saver,
            mesh.get_vertices(),
            mesh.get_faces(),
            vertex_per_face);

    for (AttrNames::const_iterator itr = m_attr_names.begin();
            itr != m_attr_names.end(); itr++) {
        if (!mesh.has_attribute(*itr)) {
            std::cerr << "Error: Attribute \"" << *itr<< "\" does not exist."
                << std::endl;
            continue;
        }

        VectorF& attr = mesh.get_attribute(*itr);
        write_attribute(saver, *itr, attr, num_vertices, num_faces);
    }
}

void MSHWriter::write_volume_mesh(Mesh& mesh) {
    Zhou::MshSaver saver(m_filename, true);

    size_t dim = mesh.get_dim();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_voxels = mesh.get_num_voxels();
    size_t vertex_per_voxel = mesh.get_vertex_per_voxel();

    write_geometry(saver,
            mesh.get_vertices(),
            mesh.get_voxels(),
            vertex_per_voxel);

    for (AttrNames::const_iterator itr = m_attr_names.begin();
            itr != m_attr_names.end(); itr++) {
        if (!mesh.has_attribute(*itr)) {
            std::cerr << "Error: Attribute \"" << *itr<< "\" does not exist."
                << std::endl;
            continue;
        }

        VectorF& attr = mesh.get_attribute(*itr);
        write_attribute(saver, *itr, attr, num_vertices, num_voxels);
    }
}

void MSHWriter::write_geometry(Zhou::MshSaver& saver, VectorF& vertices,
        VectorI& elements, size_t vertex_per_element) {
    saver.save_mesh(vertices, elements, 3, vertex_per_element);
}

void MSHWriter::write_attribute(Zhou::MshSaver& saver, const std::string& name,
        VectorF& value, size_t num_vertices, size_t num_elements) {
    size_t attr_size = value.size();

    if (attr_size == num_vertices) {
        saver.save_scalar_field(name, value);
    } else if (attr_size == num_vertices * 3) {
        saver.save_vector_field(name, value);
    } else if (attr_size == num_elements) {
        saver.save_elem_scalar_field(name, value);
    } else if (attr_size == num_elements * 3) {
        saver.save_elem_vector_field(name, value);
    } else {
        std::cerr << "Unknow attribute type." << std::endl;
    }
}




