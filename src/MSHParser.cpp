#include "MSHParser.h"

#include <map>

#include "MshLoader.h"
#include "Triplet.h"

using namespace Zhou;

bool MSHParser::parse(const std::string& filename) {
    m_loader = LoaderPtr(new MshLoader(filename));
    extract_surface_from_volume();
    return true;
}

size_t MSHParser::num_vertices() const {
    const VectorF& vertices = m_loader->get_nodes();
    return vertices.size() / 3;
}

size_t MSHParser::num_faces() const {
    return m_faces.size();
}

size_t MSHParser::num_voxels() const {
    const VectorI& voxels = m_loader->get_elements();
    return voxels.size() / m_loader->get_nodes_per_element();
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
    size_t counter = 0;
    size_t face_degree = vertex_per_face();
    for (FaceList::const_iterator itr = m_faces.begin();
            itr != m_faces.end(); itr++) {
        std::copy(itr->data(), itr->data() + face_degree, &buffer[counter]);
        counter += face_degree;
    }
}

void MSHParser::export_voxels(int* buffer) {
    const VectorI& voxels = m_loader->get_elements();
    std::copy(voxels.data(), voxels.data() + voxels.size(), buffer);
}

void MSHParser::export_attribute(const std::string& name, Float* buffer) {
    const VectorF& attribute = get_attribute(name);
    std::copy(attribute.data(), attribute.data() + attribute.size(), buffer);
}

size_t MSHParser::vertex_per_voxel() const {
    return m_loader->get_nodes_per_element();
}

// TODO: only triangle mesh supported so far
size_t MSHParser::vertex_per_face() const {
    return 3;
}

void MSHParser::extract_surface_from_volume() {
    const VectorI& voxels = m_loader->get_elements();
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

    for (FaceCounter::const_iterator itr = face_counter.begin();
            itr!=face_counter.end(); itr++) {
        assert(itr->second == 1 or itr->second == 2);
        if (itr->second == 1) {
            m_faces.push_back(itr->first.get_ori_data());
        }
    }
}

const VectorF& MSHParser::get_attribute(const std::string& name) const {
    if (m_loader->is_node_field(name)) {
        return m_loader->get_node_field(name);
    } else {
        return m_loader->get_element_field(name);
    }
}
