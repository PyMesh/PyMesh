#include "POLYParser.h"
#include "IOUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

bool POLYParser::parse(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    if (!fin.good()) {
        std::stringstream err_msg;
        err_msg << "Unable to read file " << filename;
        throw IOError(err_msg.str());
    }

    parse_vertices(fin);
    parse_faces(fin);

    fin.close();
    return true;
}

POLYParser::AttrNames POLYParser::get_attribute_names() const {
    return AttrNames();
}

void POLYParser::export_vertices(Float* buffer) {
    std::copy(m_vertices.begin(), m_vertices.end(), buffer);
}

void POLYParser::export_faces(int* buffer) {
    std::copy(m_faces.begin(), m_faces.end(), buffer);
}

void POLYParser::export_voxels(int* buffer) {
    std::copy(m_voxels.begin(), m_voxels.end(), buffer);
}

void POLYParser::export_attribute(const std::string& name, Float* buffer) {
    std::cerr << "Warning: attribute "  << name << " does not exist." <<
        std::endl;
}

void POLYParser::parse_vertices(std::ifstream& fin) {
    std::string header = IOUtils::next_line(fin);
    size_t num_vertices, dim, num_attributes, bd_marker;
    sscanf(header.c_str(), "%zi %zi %zi %zi",
            &num_vertices, &dim, &num_attributes, &bd_marker);

    if (dim != 3) {
        throw IOError("Dim is not 3!");
    }

    m_vertices.resize(num_vertices * dim);
    for (size_t i=0; i<num_vertices; i++) {
        assert(fin.good());
        size_t index;
        Float x,y,z;
        fin >> index >> x >> y >> z;
        assert(index >= 0);
        assert(index < num_vertices);
        m_vertices[index*3  ] = x;
        m_vertices[index*3+1] = y;
        m_vertices[index*3+2] = z;
        for (size_t j=0; j<num_attributes; j++) {
            Float attr;
            fin >> attr;
        }
        if (bd_marker == 1) {
            Float bd;
            fin >> bd;
        }
    }
}

void POLYParser::parse_faces(std::ifstream& fin) {
    size_t num_faces, bd_marker;
    fin >> num_faces >> bd_marker;

    for (size_t i=0; i<num_faces; i++) {
        assert(fin.good());
        std::string line = IOUtils::next_line(fin);
        size_t num_poly=0, num_holes=0, bd=0;
        size_t n = sscanf(line.c_str(), "%zi %zi %zi",
                &num_poly, &num_holes, &bd);
        assert(n >= 1);
        for (size_t j=0; j<num_poly; j++) {
            size_t num_corners;
            fin >> num_corners;
            if (i == 0 && j == 0) {
                m_vertex_per_face = num_corners;
            } else if (num_corners != m_vertex_per_face) {
                throw NotImplementedError("Mixed faces is not supported!");
            }

            for (size_t k=0; k<num_corners; k++) {
                size_t corner;
                fin >> corner;
                m_faces.push_back(corner);
            }
        }
        assert(fin.good());
        for (size_t j=0; j<num_holes; j++) {
            size_t hole_index;
            float x,y,z;
            fin >> hole_index >> x >> y >> z;
        }
    }
}

