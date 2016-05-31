/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MSHParser.h"

#include <map>
#include <sstream>
#include <vector>

#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <Misc/Multiplet.h>

#include "MshLoader.h"

using namespace PyMesh;

bool MSHParser::parse(const std::string& filename) {
    m_loader = LoaderPtr(new MshLoader(filename));
    extract_faces_and_voxels();
    return true;
}

size_t MSHParser::num_vertices() const {
    const VectorF& vertices = m_loader->get_nodes();
    assert(vertices.size() % 3 == 0);
    return vertices.size() / 3;
}

size_t MSHParser::num_faces() const {
    const size_t face_size = m_faces.size();
    const size_t v_per_face = vertex_per_face();
    assert(face_size % v_per_face == 0);
    return face_size / v_per_face;
}

size_t MSHParser::num_voxels() const {
    const size_t voxel_size = m_voxels.size();
    const size_t v_per_voxel = vertex_per_voxel();

    if (voxel_size == 0)
        return 0;
    else {
        assert(v_per_voxel > 0);
        assert(voxel_size % v_per_voxel== 0);
        return voxel_size / v_per_voxel;
    }
}

size_t MSHParser::num_attributes() const {
    MshLoader::FieldNames node_names = m_loader->get_node_field_names();
    MshLoader::FieldNames elem_names = m_loader->get_element_field_names();
    return node_names.size() + elem_names.size();
}

MSHParser::AttrNames MSHParser::get_attribute_names() const {
    MSHParser::AttrNames attr_names;
    MshLoader::FieldNames node_names = m_loader->get_node_field_names();
    MshLoader::FieldNames elem_names = m_loader->get_element_field_names();
    attr_names.insert(attr_names.end(), node_names.begin(), node_names.end());
    attr_names.insert(attr_names.end(), elem_names.begin(), elem_names.end());
    return attr_names;
}

size_t MSHParser::get_attribute_size(const std::string& name) const {
    const VectorF& attribute = get_attribute(name);
    return attribute.size();
}

void MSHParser::export_vertices(Float* buffer) {
    const VectorF& vertices = m_loader->get_nodes();
    std::copy(vertices.data(), vertices.data() + vertices.size(), buffer);
}

void MSHParser::export_faces(int* buffer) {
    const VectorI& faces = m_faces;
    std::copy(faces.data(), faces.data() + faces.size(), buffer);
}

void MSHParser::export_voxels(int* buffer) {
    const VectorI& voxels = m_voxels;
    std::copy(voxels.data(), voxels.data() + voxels.size(), buffer);
}

void MSHParser::export_attribute(const std::string& name, Float* buffer) {
    const VectorF& attribute = get_attribute(name);
    std::copy(attribute.data(), attribute.data() + attribute.size(), buffer);
}

size_t MSHParser::vertex_per_voxel() const {
    return m_vertex_per_voxel;
}

size_t MSHParser::vertex_per_face() const {
    return m_vertex_per_face;
}

void MSHParser::extract_faces_and_voxels() {
    const size_t element_type = m_loader->get_element_type();
    switch (element_type) {
        case 2: // Triangle
            m_vertex_per_face = 3;
            m_vertex_per_voxel = 0;
            m_faces = m_loader->get_elements();
            break;
        case 3: // Quad
            m_vertex_per_face = 4;
            m_vertex_per_voxel = 0;
            m_faces = m_loader->get_elements();
            break;
        case 4: // Tetrahedron
            m_vertex_per_face = 3;
            m_vertex_per_voxel = 4;
            m_voxels = m_loader->get_elements();
            extract_surface_from_tets();
            break;
        case 5: // Hexahedron
            m_vertex_per_face = 4;
            m_vertex_per_voxel = 8;
            m_voxels = m_loader->get_elements();
            extract_surface_from_hexs();
            break;
        default:
            std::stringstream err_msg;
            err_msg << "Unsupported msh element type: " << element_type;
            throw NotImplementedError(err_msg.str());
    }
}

void MSHParser::extract_surface_from_tets() {
    const VectorI& voxels = m_voxels;
    const size_t num_vertex_per_voxel = vertex_per_voxel();
    typedef std::map<Triplet, int> FaceCounter;
    FaceCounter face_counter;

    for (size_t i=0; i<voxels.size(); i+= num_vertex_per_voxel) {
        VectorI voxel = voxels.segment(i, num_vertex_per_voxel);
        // TODO: only tet mesh is handled.
        // Note that the order of vertices below are predefined by MSH format,
        // each face should have normal pointing outward.
        assert(voxel.size() == 4);
        Triplet voxel_faces[4] = {
            Triplet(voxel[0], voxel[2], voxel[1]),
            Triplet(voxel[0], voxel[1], voxel[3]),
            Triplet(voxel[0], voxel[3], voxel[2]),
            Triplet(voxel[1], voxel[2], voxel[3])
        };
        for (size_t j=0; j<4; j++) {
            if (face_counter.find(voxel_faces[j]) == face_counter.end()) {
                face_counter[voxel_faces[j]] = 1;
            } else {
                face_counter[voxel_faces[j]] += 1;
            }
        }
    }

    std::vector<int> vertex_buffer;
    for (FaceCounter::const_iterator itr = face_counter.begin();
            itr!=face_counter.end(); itr++) {
        assert(itr->second == 1 or itr->second == 2);
        if (itr->second == 1) {
            const VectorI& f = itr->first.get_ori_data();
            // TODO: only triangles is handled.
            assert(f.size() == 3);
            vertex_buffer.push_back(f[0]);
            vertex_buffer.push_back(f[1]);
            vertex_buffer.push_back(f[2]);
        }
    }

    m_faces.resize(vertex_buffer.size());
    std::copy(vertex_buffer.begin(), vertex_buffer.end(), m_faces.data());
}

void MSHParser::extract_surface_from_hexs() {
    const VectorI& voxels = m_voxels;
    const size_t num_vertex_per_voxel = vertex_per_voxel();
    assert(num_vertex_per_voxel == 8);
    typedef std::map<Multiplet, int> FaceCounter;
    FaceCounter face_counter;

    for (size_t i=0; i<voxels.size(); i+=num_vertex_per_voxel) {
        const VectorI voxel = voxels.segment(i, num_vertex_per_voxel);
        Multiplet voxel_faces[6] = {
            Multiplet(voxel[0], voxel[4], voxel[7], voxel[3]),
            Multiplet(voxel[1], voxel[2], voxel[6], voxel[5]),
            Multiplet(voxel[2], voxel[3], voxel[7], voxel[6]),
            Multiplet(voxel[0], voxel[1], voxel[5], voxel[4]),
            Multiplet(voxel[0], voxel[3], voxel[2], voxel[1]),
            Multiplet(voxel[4], voxel[5], voxel[6], voxel[7])
        };
        for (size_t j=0; j<6; j++) {
            if (face_counter.find(voxel_faces[j]) == face_counter.end()) {
                face_counter[voxel_faces[j]] = 1;
            } else {
                face_counter[voxel_faces[j]] += 1;
            }
        }
    }

    std::vector<int> vertex_buffer;
    for (FaceCounter::const_iterator itr = face_counter.begin();
            itr!=face_counter.end(); itr++) {
        if (itr->second == 1) {
            const VectorI& f = itr->first.get_ori_data();
            vertex_buffer.push_back(f[0]);
            vertex_buffer.push_back(f[1]);
            vertex_buffer.push_back(f[2]);
            vertex_buffer.push_back(f[3]);
        }
    }

    m_faces.resize(vertex_buffer.size());
    std::copy(vertex_buffer.begin(), vertex_buffer.end(), m_faces.data());
}

const VectorF& MSHParser::get_attribute(const std::string& name) const {
    if (m_loader->is_node_field(name)) {
        return m_loader->get_node_field(name);
    } else {
        return m_loader->get_element_field(name);
    }
}
