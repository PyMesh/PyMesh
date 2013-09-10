#include "MSHParser.h"

#include <map>
#include <vector>
#include <iostream>

#include "MshLoader.h"
#include "Triplet.h"
#include "Exception.h"

using namespace Zhou;

bool MSHParser::parse(const std::string& filename) {
    m_loader = LoaderPtr(new MshLoader(filename));
    extract_faces_and_voxels();
    return true;
}

size_t MSHParser::num_vertices() const {
    const VectorF& vertices = m_loader->get_nodes();
    return vertices.size() / 3;
}

size_t MSHParser::num_faces() const {
    return m_faces.size() / 3;
}

size_t MSHParser::num_voxels() const {
    return m_voxels.size() / 4;
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

// TODO: only tet mesh supported so far
size_t MSHParser::vertex_per_voxel() const {
    return 4;
}

// TODO: only triangle mesh supported so far
size_t MSHParser::vertex_per_face() const {
    return 3;
}

void MSHParser::extract_faces_and_voxels() {
    size_t nodes_per_element = m_loader->get_nodes_per_element();
    if (nodes_per_element == 3) {
        std::cout << "surface" << std::endl;
        m_faces = m_loader->get_elements();
        std::cout << m_faces << std::endl;
    } else if (nodes_per_element == 4) {
        std::cout << "volume" << std::endl;
        m_voxels = m_loader->get_elements();
        extract_surface_from_volume();
    } else {
        throw NotImplementedError("Only triangle and tet mesh is supported.");
    }
}

void MSHParser::extract_surface_from_volume() {
    const VectorI& voxels = m_voxels;
    size_t num_vertex_per_voxel = vertex_per_voxel();
    typedef std::map<Triplet, int> FaceCounter;
    FaceCounter face_counter;

    for (size_t i=0; i<voxels.size(); i+= num_vertex_per_voxel) {
        VectorI voxel = voxels.segment(i, num_vertex_per_voxel);
        // TODO: only tet mesh is handled.
        // Note that the order of vertices below are predefined by MSH format,
        // each face should have normal pointing outward.
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
            const Vector3I& f = itr->first.get_ori_data();
            vertex_buffer.push_back(f[0]);
            vertex_buffer.push_back(f[1]);
            vertex_buffer.push_back(f[2]);
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
