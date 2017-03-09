/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include "NodeParser.h"
#include "IOUtils.h"

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Triplet.h>

using namespace PyMesh;

namespace NodeParserHelper {
    void next_line(std::ifstream& fin, char* line, size_t LINE_SIZE) {
        do {
            if (fin.eof()) {
                throw IOError("Error parsing .node/.face/.ele file");
            }
            fin.getline(line, LINE_SIZE);
        } while(strlen(line) == 0 || line[0] == '#' ||
                line[0] == '\n' || line[0] == '\r');
    }
}

using namespace NodeParserHelper;

NodeParser::NodeParser() :
    m_num_vertices(0),
    m_num_faces(0),
    m_num_voxels(0),
    m_dim(3),
    m_vertex_per_face(3),
    m_vertex_per_voxel(4),
    m_num_node_attributes(0) {}

bool NodeParser::parse(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    const std::string& node_file = filename;
    const std::string name = IOUtils::get_name(filename);
    const std::string face_file = name + ".face";
    const std::string ele_file = name + ".ele";

    bool node_success = parse_nodes(node_file);
    bool face_success = parse_faces(face_file);
    bool elem_success = parse_elements(ele_file);
    if (!node_success) {
        throw IOError("Parsing .node file failed.");
    }
    if (!face_success && !elem_success) {
        std::cerr << "Warning: failed to parse .face or .ele files." 
            << std::endl;
    }
    if (!face_success && elem_success) {
        extract_faces_from_voxels();
    }
    return true;
}

bool NodeParser::parse_nodes(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        return false;
    }

    size_t boundary_marker;
    next_line(fin, line, LINE_SIZE);
    size_t n = sscanf(line,
            "%zd %zd %zd %zd", &m_num_vertices, &m_dim,
            &m_num_node_attributes, &boundary_marker);
    assert(boundary_marker == 0 || boundary_marker == 1);

    for (size_t i=0; i<m_num_vertices; i++) {
        next_line(fin, line, LINE_SIZE);
        std::stringstream ss;
        ss << line;
        VectorF v(m_dim);
        size_t index;
        ss >> index;
        for (size_t j=0; j<m_dim; j++) {
            ss >> v[j];
        }
        m_vertices.push_back(std::move(v));

        if (m_num_node_attributes> 0) {
            VectorF attr(m_num_node_attributes);
            for (size_t j=0; j<m_num_node_attributes; j++) {
                ss >> attr[j];
            }
            m_node_attributes.push_back(std::move(attr));
        }

        if (boundary_marker == 1) {
            int bd_marker;
            ss >> bd_marker;
            m_boundary_node_markers.push_back(bd_marker);
        }
    }

    return true;
}

bool NodeParser::parse_faces(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    // .face file only supports triangles.
    m_vertex_per_face = 3;

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        return false;
    }

    size_t boundary_marker;
    next_line(fin, line, LINE_SIZE);
    sscanf(line, "%zd %zd", &m_num_faces, &boundary_marker);
    assert(boundary_marker == 0 || boundary_marker == 1);

    for (size_t i=0; i<m_num_faces; i++) {
        next_line(fin, line, LINE_SIZE);
        std::stringstream ss;
        ss << line;
        VectorI f(3);
        size_t index;
        ss >> index >> f[0] >> f[1] >> f[2];
        m_faces.push_back(std::move(f));

        if (boundary_marker == 1) {
            int bd_marker;
            ss >> bd_marker;
            m_boundary_face_markers.push_back(bd_marker);
        }
    }
    return true;
}

bool NodeParser::parse_elements(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        return false;
    }

    size_t region_attribute;
    next_line(fin, line, LINE_SIZE);
    sscanf(line, "%zd %zd %zd", &m_num_voxels, &m_vertex_per_voxel,
            &region_attribute);
    assert(m_vertex_per_voxel == 4 || m_vertex_per_voxel == 10);
    if (m_vertex_per_voxel != 4) {
        // High order tet elements.
        throw NotImplementedError(
                "Tetrahedron with 10 nodes is not supported");
    }

    for (size_t i=0; i<m_num_voxels; i++) {
        next_line(fin, line, LINE_SIZE);
        std::stringstream ss;
        ss << line;
        size_t index;
        VectorI voxel(4);
        ss >> index >> voxel[0] >> voxel[1] >> voxel[2] >> voxel[3];
        m_voxels.push_back(std::move(voxel));

        if (region_attribute == 1) {
            int region;
            ss >> region;
            m_region_attributes.push_back(region);
        }
    }
    return true;
}

size_t NodeParser::num_attributes() const {
    return m_num_node_attributes +
        (m_region_attributes.empty() ? 0:1) +
        (m_boundary_node_markers.empty() ? 0:1) +
        (m_boundary_face_markers.empty() ? 0:1);
}

NodeParser::AttrNames NodeParser::get_attribute_names() const {
    AttrNames names;
    for (size_t i=0; i<m_num_node_attributes; i++) {
        std::stringstream name;
        name << "node_attribute_" << i;
        names.push_back(name.str());
    }
    if (!m_boundary_node_markers.empty()) {
        names.push_back("node_boundary_marker");
    }
    if (!m_boundary_face_markers.empty()) {
        names.push_back("face_boundary_marker");
    }
    if (!m_region_attributes.empty()) {
        names.push_back("region");
    }
    return names;
}

size_t NodeParser::get_attribute_size(const std::string& name) const {
    if (name.substr(0, 4) == "node") {
        return m_num_vertices;
    } else if (name.substr(0, 4) == "face") {
        return m_num_faces;
    } else {
        assert(name == "region");
        return m_num_voxels;
    }
}

void NodeParser::export_vertices(Float* buffer) {
    const size_t dim = m_dim;
    size_t count=0;
    for (VertexList::const_iterator vi=m_vertices.begin();
            vi != m_vertices.end(); vi++) {
        const VectorF& v = *vi;
        for (size_t i=0; i<dim; i++) {
            buffer[dim * count + i] = v[i];
        }
        count++;
    }
}

void NodeParser::export_faces(int* buffer) {
    size_t count=0;
    for (FaceList::const_iterator fi=m_faces.begin();
            fi != m_faces.end(); fi++) {
        const VectorI& f = *fi;
        assert(f.size() == m_vertex_per_face);
        std::copy(f.data(), f.data() + m_vertex_per_face,
                buffer + count*m_vertex_per_face);
        count++;
    }
}

void NodeParser::export_voxels(int* buffer) {
    size_t count = 0;
    for (VoxelList::const_iterator vi=m_voxels.begin();
            vi != m_voxels.end(); vi++) {
        const VectorI& v = *vi;
        assert(v.size() == m_vertex_per_voxel);
        std::copy(v.data(), v.data() + m_vertex_per_voxel,
                buffer + count * m_vertex_per_voxel);
        count++;
    }
}

void NodeParser::export_attribute(const std::string& name, Float* buffer) {
    if (IOUtils::is_prefix("node_attribute_", name.c_str())) {
        int index = -1;
        sscanf(name.c_str(), "node_attribute_%d", &index);
        assert(index >=0 && index < m_num_node_attributes);
        assert(m_node_attributes.size() == m_num_vertices);
        size_t count = 0;
        for (const auto& attr : m_node_attributes) {
            buffer[count] = attr[index];
            count++;
        }
    } else if (name == "node_boundary_marker") {
        std::copy(m_boundary_node_markers.begin(),
                m_boundary_node_markers.end(), buffer);
    } else if (name == "face_boundary_marker") {
        std::copy(m_boundary_face_markers.begin(),
                m_boundary_face_markers.end(), buffer);
    } else if (name == "region") {
        std::copy(m_region_attributes.begin(),
                m_region_attributes.end(), buffer);
    } else {
        std::stringstream err_msg;
        err_msg << "Unknown attribute: " << name;
        throw RuntimeError(err_msg.str());
    }
}

void NodeParser::extract_faces_from_voxels() {
    if (m_vertex_per_voxel != 4) {
        throw NotImplementedError("Only tet element is supported");
    }
    m_faces.clear();

    typedef std::map<Triplet, int> FaceCounter;
    FaceCounter face_counter;
    const size_t num_voxels = m_voxels.size();
    for (const auto& voxel : m_voxels) {
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
            m_faces.push_back(f);
        }
    }
    m_num_faces = m_faces.size();
    m_vertex_per_face = 3;
}
