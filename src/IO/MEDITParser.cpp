#include "MEDITParser.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

bool MEDITParser::parse(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::stringstream err_msg;
        err_msg << "failed to open file \"" << filename << "\"";
        throw IOError(err_msg.str());
    }

    bool success = true;
    success &= parse_header(fin);
    if (!success) {
        throw IOError("Error parsing file header.");
    }

    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];
    while (!fin.eof()) {
        std::string elem_type;
        fin >> elem_type;
        if (elem_type.size() == 0) continue;
        if (elem_type[0] == '#') {
            fin.getline(line, LINE_SIZE);
            continue;
        }
        if (elem_type == "Vertices") {
            success &= parse_vertices(fin);
        } else if (elem_type == "Triangles") {
            m_vertex_per_face = 3;
            success &= parse_faces(fin);
        } else if (elem_type == "Quadrilaterals") {
            m_vertex_per_face = 4;
            success &= parse_faces(fin);
        } else if (elem_type == "Tetrahedra") {
            if (m_faces.size() == 0) {
                m_vertex_per_face = 3;
            } else if (m_vertex_per_face != 3){
                success = false;
            }
            m_vertex_per_voxel = 4;
            success &= parse_voxels(fin);
        } else if (elem_type == "Hexahedra") {
            if (m_faces.size() == 0) {
                m_vertex_per_face = 4;
            } else if (m_vertex_per_face != 4){
                success = false;
            }
            m_vertex_per_voxel = 8;
            success &= parse_voxels(fin);
        } else if (elem_type == "End") {
            break;
        } else {
            if (elem_type != "Edges" &&
                elem_type != "Corners" &&
                elem_type != "RequiredVertices" &&
                elem_type != "Ridges" &&
                elem_type != "RequiredEdges" &&
                elem_type != "Normals" &&
                elem_type != "Tangents" &&
                elem_type != "NormalAtVertices" &&
                elem_type != "NormalAtTriangleVertices" &&
                elem_type != "NormalAtQuadrilateralVertices" &&
                elem_type != "TangentAtEdges" &&
                elem_type != "End") {
                success = false;
            } else {
                std::cerr << "Warning: Skipping " << elem_type
                    << " field" << std::endl;
                success &= skip_field(fin);
            }
        }
        if (!success) {
            std::stringstream err_msg;
            err_msg << "Error parsing \"" << elem_type << "\" field in " <<
                filename;
            throw IOError(err_msg.str());
        }
    }

    fin.close();
    return success;
}

size_t MEDITParser::num_faces() const {
    if (m_vertex_per_face == 0) return 0;
    else return m_faces.size() / m_vertex_per_face;
}

size_t MEDITParser::num_voxels() const {
    if (m_vertex_per_voxel == 0) return 0;
    else return m_voxels.size() / m_vertex_per_voxel;
}

MEDITParser::AttrNames MEDITParser::get_attribute_names() const {
    MEDITParser::AttrNames attr_names;
    return attr_names;
}

void MEDITParser::export_vertices(Float* buffer) {
    std::copy(m_vertices.begin(), m_vertices.end(), buffer);
}

void MEDITParser::export_faces(int* buffer) {
    std::copy(m_faces.begin(), m_faces.end(), buffer);
}

void MEDITParser::export_voxels(int* buffer) {
    std::copy(m_voxels.begin(), m_voxels.end(), buffer);
}

void MEDITParser::export_attribute(const std::string& name, Float* buffer) {
    std::cerr << "Warning: attribute "  << name << " does not exist." <<
        std::endl;
}

bool MEDITParser::parse_header(std::ifstream& fin) {
    std::string str_val;
    int int_val;
    fin >> str_val >> int_val;
    if (str_val != "MeshVersionFormatted" || int_val != 1) {
        return false;
    }
    fin >> str_val >> int_val;
    if (str_val != "Dimension" || int_val != 3) {
        return false;
    }
    return true;
}

bool MEDITParser::parse_vertices(std::ifstream& fin) {
    size_t num_vertices=0;
    fin >> num_vertices;
    Float val;
    int ref;
    for (size_t i=0; i<num_vertices; i++) {
        for (size_t j=0; j<3; j++) {
            fin >> val;
            m_vertices.push_back(val);
        }
        fin >> ref;
        if (!fin.good()) return false;
    }
    return true;
}

bool MEDITParser::parse_faces(std::ifstream& fin) {
    size_t num_faces=0;
    fin >> num_faces;
    int val, ref;
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<m_vertex_per_face; j++) {
            fin >> val;
            m_faces.push_back(val-1);
        }
        fin >> ref;
        if (!fin.good()) return false;
    }
    return true;
}

bool MEDITParser::parse_voxels(std::ifstream& fin) {
    size_t num_voxels=0;
    fin >> num_voxels;
    int val, ref;
    for (size_t i=0; i<num_voxels; i++) {
        for (size_t j=0; j<m_vertex_per_voxel; j++) {
            fin >> val;
            m_voxels.push_back(val-1);
        }
        fin >> ref;
        if (!fin.good()) return false;
    }
    return true;
}

bool MEDITParser::skip_field(std::ifstream& fin) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    size_t num_lines=0;
    fin >> num_lines;
    for (size_t i=0; i<num_lines; i++) {
        fin.getline(line, LINE_SIZE);
        if (!fin.good()) return false;
    }
    return true;
}
