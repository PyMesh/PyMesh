#include "MSHWriter.h"

#include <iostream>
#include <fstream>

#include <Core/Exception.h>
#include <Mesh.h>

#include "MshSaver.h"

namespace MSHWriterHelper {
    bool positive_orientated(
            const Vector3F& v1, const Vector3F& v2,
            const Vector3F& v3, const Vector3F& v4) {
        return (v2 - v1).cross(v3 - v1).dot(v4 - v1) >= 0.0;
    }

    void correct_tet_orientation(const VectorF& vertices, VectorI& voxels) {
        const size_t num_voxels = voxels.size() / 4;
        for (size_t i=0; i<num_voxels; i++) {
            const VectorI tet = voxels.segment(i*4, 4);
            const Vector3F& v1 = vertices.segment(tet[0]*3, 3);
            const Vector3F& v2 = vertices.segment(tet[1]*3, 3);
            const Vector3F& v3 = vertices.segment(tet[2]*3, 3);
            const Vector3F& v4 = vertices.segment(tet[3]*3, 3);
            if (!positive_orientated(v1, v2, v3, v4)) {
                voxels[i*4]   = tet[1];
                voxels[i*4+1] = tet[0];
            }
        }
    }
}

using namespace MSHWriterHelper;

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
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    Zhou::MshSaver saver(m_filename, true);
    if (voxels.size() == 0) {
        write_geometry(saver, vertices, faces, dim, vertex_per_face);
    } else {
        write_geometry(saver, vertices, voxels, dim, vertex_per_voxel);
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
            dim,
            vertex_per_face);

    for (AttrNames::const_iterator itr = m_attr_names.begin();
            itr != m_attr_names.end(); itr++) {
        if (!mesh.has_attribute(*itr)) {
            std::cerr << "Error: Attribute \"" << *itr<< "\" does not exist."
                << std::endl;
            continue;
        }

        VectorF& attr = mesh.get_attribute(*itr);
        write_attribute(saver, *itr, attr, dim, num_vertices, num_faces);
    }
}

void MSHWriter::write_volume_mesh(Mesh& mesh) {
    Zhou::MshSaver saver(m_filename, true);

    size_t dim = mesh.get_dim();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_voxels = mesh.get_num_voxels();
    size_t vertex_per_voxel = mesh.get_vertex_per_voxel();

    if (vertex_per_voxel != 4) {
        throw NotImplementedError("Only tet mesh is supported!");
    }
    correct_tet_orientation(mesh.get_vertices(), mesh.get_voxels());

    write_geometry(saver,
            mesh.get_vertices(),
            mesh.get_voxels(),
            dim,
            vertex_per_voxel);

    for (AttrNames::const_iterator itr = m_attr_names.begin();
            itr != m_attr_names.end(); itr++) {
        if (!mesh.has_attribute(*itr)) {
            std::cerr << "Error: Attribute \"" << *itr<< "\" does not exist."
                << std::endl;
            continue;
        }

        VectorF& attr = mesh.get_attribute(*itr);
        write_attribute(saver, *itr, attr, dim, num_vertices, num_voxels);
    }
}

void MSHWriter::write_geometry(Zhou::MshSaver& saver, VectorF& vertices,
        VectorI& elements, size_t dim, size_t vertex_per_element) {
    if (vertex_per_element == 4) {
        correct_tet_orientation(vertices, elements);
    }
    saver.save_mesh(vertices, elements, dim, vertex_per_element);
}

void MSHWriter::write_attribute(Zhou::MshSaver& saver, const std::string& name,
        VectorF& value, size_t dim, size_t num_vertices, size_t num_elements) {
    size_t attr_size = value.size();

    if (attr_size == num_vertices) {
        saver.save_scalar_field(name, value);
    } else if (attr_size == num_vertices * dim) {
        saver.save_vector_field(name, value);
    } else if (attr_size == num_elements) {
        saver.save_elem_scalar_field(name, value);
    } else if (attr_size == num_elements * dim) {
        saver.save_elem_vector_field(name, value);
    } else {
        std::cerr << "Unknow attribute type." << std::endl;
    }
}




