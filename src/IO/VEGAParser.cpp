/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "VEGAParser.h"
#include "IOUtils.h"

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Multiplet.h>

using namespace PyMesh;

VEGAParser::VEGAParser() :
    m_num_vertices(0),
    m_num_faces(0),
    m_num_voxels(0),
    m_dim(3),
    m_vertex_per_face(3),
    m_vertex_per_voxel(4),
    m_num_node_attributes(0),
    m_index_start_from_zero(false) {}


bool VEGAParser::parse(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::stringstream err_msg;
        err_msg << "failed to open file \"" << filename << "\"";
        throw IOError(err_msg.str());
    }

    while(!fin.eof()) {
        const std::string line = IOUtils::next_line(fin);
        assert(line.size() > 0);

        if (line[0] == '*') {
            if (IOUtils::is_prefix("*VERTICES", line.c_str())) {
                parse_vertices(fin);
            } else if (IOUtils::is_prefix("*ELEMENTS", line.c_str())) {
                parse_elements(fin);
            } else if (IOUtils::is_prefix("*MATERIAL", line.c_str())) {
                parse_material(fin);
            } else if (IOUtils::is_prefix("*REGION", line.c_str())) {
                parse_region(fin);
            } else if (IOUtils::is_prefix("*SET", line.c_str())) {
                parse_set(fin);
            } else {
                std::stringstream err_msg;
                err_msg << "Unknown VEGA command: " << line;
                throw IOError(err_msg.str());
            }
        } else {
            std::cerr << "Warning: unknown line skipped: " << line << std::endl;
        }
    }
    extract_faces_from_voxels();
    return true;
}

void VEGAParser::parse_vertices(std::ifstream& fin) {
    const std::string header = IOUtils::next_line(fin);
    std::stringstream header_stream(header);
    header_stream >> m_num_vertices >> m_dim;
    for (size_t i=0; i<m_num_vertices; i++) {
        if (fin.eof()) {
            throw IOError("Error parsing VEGA vertices, EOF reached!");
        }
        const std::string line = IOUtils::next_line(fin);
        size_t index;
        VectorF v(3);
        std::stringstream v_line(line);
        v_line >> index >> v[0] >> v[1] >> v[2];
        m_vertices.push_back(v);
        if (i == 0 && index == 0 && m_vertices.size() == 1) {
            m_index_start_from_zero = true;
        }
    }
}

void VEGAParser::parse_elements(std::ifstream& fin) {
    const std::string elem_type = IOUtils::next_line(fin);
    if (elem_type == "TETS" || elem_type == "TET") {
        m_vertex_per_face = 3;
        m_vertex_per_voxel = 4;
    } else if (elem_type == "CUBIC") {
        m_vertex_per_face = 4;
        m_vertex_per_voxel = 8;
    } else {
        std::stringstream err_msg;
        err_msg << "Unknown VEGA element type: " << elem_type;
        throw IOError(err_msg.str());
    }

    const std::string header = IOUtils::next_line(fin);
    std::stringstream header_stream(header);
    size_t vertex_per_voxel;
    header_stream >> m_num_voxels >> vertex_per_voxel;
    assert(vertex_per_voxel == m_vertex_per_voxel);

    for (size_t i=0; i<m_num_voxels; i++) {
        if (fin.eof()) {
            throw IOError("Error parsing VEGA elements, EOF reached!");
        }
        const std::string line = IOUtils::next_line(fin);
        std::stringstream e_line(line);
        size_t index;
        if (m_vertex_per_voxel == 4) {
            VectorI v(4);
            e_line >> index >> v[0] >> v[1] >> v[2] >> v[3];
            m_voxels.push_back(v);
        } else {
            assert(m_vertex_per_voxel == 8);
            VectorI v(8);
            e_line >> index
                >> v[0] >> v[1] >> v[2] >> v[3] 
                >> v[4] >> v[5] >> v[6] >> v[7];
            m_voxels.push_back(v);
        }
    }
    if (!m_index_start_from_zero) {
        for (auto& e : m_voxels) {
            e.array() -= 1;
            assert(e.minCoeff() >= 0);
            assert(e.minCoeff() < m_num_vertices);
        }
    }
}

void VEGAParser::parse_material(std::ifstream& fin) {
    std::cerr << "Warning: MATERIAL command in VEGA format is not yet supported."
        << std::endl;
    // Eat entire section.
    while(!fin.eof() && fin.peek() != '*') {
        fin.ignore(256, '\n');
    }
}

void VEGAParser::parse_region(std::ifstream& fin) {
    std::cerr << "Warning: REGION command in VEGA format is not yet supported."
        << std::endl;
    // Eat entire section.
    while(!fin.eof() && fin.peek() != '*') {
        fin.ignore(256, '\n');
    }
}

void VEGAParser::parse_include(std::ifstream& fin) {
    std::cerr << "Warning: INCLUDE command in VEGA format is not yet supported."
        << std::endl;
    // Eat entire section.
    while(!fin.eof() && fin.peek() != '*') {
        fin.ignore(256, '\n');
    }
}

void VEGAParser::parse_set(std::ifstream& fin) {
    std::cerr << "Warning: SET command in VEGA format is not yet supported."
        << std::endl;
    // Eat entire section.
    while(!fin.eof() && fin.peek() != '*') {
        fin.ignore(256, '\n');
    }
}

void VEGAParser::export_vertices(Float* buffer) {
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

void VEGAParser::export_faces(int* buffer) {
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

void VEGAParser::export_voxels(int* buffer) {
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

VEGAParser::AttrNames VEGAParser::get_attribute_names() const {
    return AttrNames();
}

size_t VEGAParser::get_attribute_size(const std::string& name) const {
    std::stringstream err_msg;
    err_msg << "Attribute " << name << " does not exists";
    throw IOError(err_msg.str());
}

void VEGAParser::export_attribute(const std::string& name, Float* buffer) {
    std::stringstream err_msg;
    err_msg << "Unknown attribute name: " << name << std::endl;
    throw IOError(err_msg.str());
}

void VEGAParser::extract_faces_from_voxels() {
    if (m_vertex_per_voxel != 4) {
        throw NotImplementedError("Only tet element is supported");
    }
    m_faces.clear();

    typedef std::map<Triplet, unsigned short> FaceCounter;
    FaceCounter face_counter;
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
        assert(itr->second == 1 || itr->second == 2);
        if (itr->second == 1) {
            const VectorI& f = itr->first.get_ori_data();
            m_faces.push_back(f);
        }
    }
    m_num_faces = m_faces.size();
    m_vertex_per_face = 3;
}
