/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MSHWriter.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <Core/Exception.h>
#include <Mesh.h>

#include "MshSaver.h"

using namespace PyMesh;

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

    MshSaver::ElementType get_face_type(size_t vertex_per_face) {
        MshSaver::ElementType type;
        switch (vertex_per_face) {
            case 3:
                type = MshSaver::TRI;
                break;
            case 4:
                type = MshSaver::QUAD;
                break;
            default:
                throw NotImplementedError("Unsupported face type");
                break;
        }
        return type;
    }

    MshSaver::ElementType get_voxel_type(size_t vertex_per_voxel) {
        MshSaver::ElementType type;
        switch (vertex_per_voxel) {
            case 4:
                type = MshSaver::TET;
                break;
            case 8:
                type = MshSaver::HEX;
                break;
            default:
                throw NotImplementedError("Unsupported voxel type");
                break;
        }
        return type;
    }
}

using namespace MSHWriterHelper;

void MSHWriter::with_attribute(const std::string& attr_name) {
    m_attr_names.push_back(attr_name);
}

void MSHWriter::write_mesh(Mesh& mesh) {
    if (mesh.get_num_voxels() == 0) {
        write_surface_mesh(mesh);
    } else {
        write_volume_mesh(mesh);
    }
}

void MSHWriter::write(const VectorF& vertices, const VectorI& faces, const VectorI& voxels,
        size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
    MshSaver saver(m_filename, !m_in_ascii);
    MshSaver::ElementType type;
    if (voxels.size() == 0) {
        type = get_face_type(vertex_per_face);
        saver.save_mesh(vertices, faces, dim, type);
    } else {
        type = get_voxel_type(vertex_per_voxel);
        saver.save_mesh(vertices, voxels, dim, type);
    }
    if (m_attr_names.size() != 0) {
        std::cerr << "Warning: all attributes are ignored." << std::endl;
    }
}


void MSHWriter::write_surface_mesh(Mesh& mesh) {
    MshSaver saver(m_filename, !m_in_ascii);

    size_t dim = mesh.get_dim();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_faces = mesh.get_num_faces();
    size_t vertex_per_face = mesh.get_vertex_per_face();

    saver.save_mesh(mesh.get_vertices(), mesh.get_faces(), dim,
            get_face_type(vertex_per_face));

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
    MshSaver saver(m_filename, !m_in_ascii);

    size_t dim = mesh.get_dim();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_voxels = mesh.get_num_voxels();
    size_t vertex_per_voxel = mesh.get_vertex_per_voxel();

    saver.save_mesh(mesh.get_vertices(), mesh.get_voxels(), dim,
            get_voxel_type(vertex_per_voxel));

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

void MSHWriter::write_attribute(MshSaver& saver, const std::string& name,
        VectorF& value, size_t dim, size_t num_vertices, size_t num_elements) {
    size_t attr_size = value.size();

    if (attr_size == num_vertices) {
        saver.save_scalar_field(name, value);
    } else if (attr_size == num_vertices * dim) {
        saver.save_vector_field(name, value);
    } else if (attr_size == num_vertices * (dim * (dim+1)) / 2 &&
            attr_size % num_elements != 0) {
        throw NotImplementedError("Per-vertex tensor field is not supported.");
    } else if (attr_size == num_elements) {
        saver.save_elem_scalar_field(name, value);
    } else if (attr_size == num_elements * dim) {
        saver.save_elem_vector_field(name, value);
    } else if (attr_size == num_elements * (dim * (dim + 1)) / 2) {
        saver.save_elem_tensor_field(name, value);
    } else {
        std::stringstream err_msg;
        err_msg << "Attribute " << name << " has length " << attr_size << std::endl;
        err_msg << "Unable to interpret the attribute type.";
        std::cerr << "Warning: ";
        std::cerr << err_msg.str() << std::endl;
    }
}
