/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OFFParser.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

namespace OFFParserHelper {
    void next_line(std::ifstream& fin, char* line, size_t LINE_SIZE) {
        do {
            if (fin.eof()) {
                throw IOError("Error parsing OFF file");
            }
            fin.getline(line, LINE_SIZE);
        } while(strlen(line) == 0 || line[0] == '#' ||
                line[0] == '\n' || line[0] == '\r');
    }

    size_t lookup_color_index(const std::string& name) {
        if (name == "red") return 0;
        if (name == "green") return 1;
        if (name == "blue") return 2;
        if (name == "alpha") return 3;
        std::stringstream err_msg;
        err_msg << "Unknown color component: " << name;
        throw RuntimeError(err_msg.str());
    }
}
using namespace OFFParserHelper;

OFFParser::OFFParser() :
    m_dim(3), m_vertex_per_face(0), m_num_vertices(0), m_num_faces(0) {
}

bool OFFParser::parse(const std::string& filename) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::stringstream err_msg;
        err_msg << "failed to open file \"" << filename << "\"";
        throw IOError(err_msg.str());
    }

    next_line(fin, line, LINE_SIZE);
    check_header(line);
    next_line(fin, line, LINE_SIZE);
    parse_geometry_counts(line);

    for (size_t i=0; i<m_num_vertices; i++) {
        if (fin.eof()) {
            throw IOError("Error in parsing vertices");
        }
        next_line(fin, line, LINE_SIZE);
        parse_vertex_line(line);
    }

    for (size_t i=0; i<m_num_faces; i++) {
        if (fin.eof()) {
            throw IOError("Error in parsing faces");
        }
        next_line(fin, line, LINE_SIZE);
        parse_face_line(line);
    }

    fin.close();
    unify_faces();
    finalize_colors();

    if (m_vertices.size() == 0) {
        m_dim = 3; // default: 3D
    }
    if (m_faces.size() == 0) {
        m_vertex_per_face = 3; // default: triangle
    }
    return true;
}

size_t OFFParser::num_attributes() const {
    size_t result = 0;
    if (!m_vertex_colors.empty()) result += 4;
    if (!m_face_colors.empty()) result += 4;
    return result;
}

OFFParser::AttrNames OFFParser::get_attribute_names() const {
    OFFParser::AttrNames names;
    if (!m_vertex_colors.empty()) {
        names.push_back("vertex_red");
        names.push_back("vertex_green");
        names.push_back("vertex_blue");
        names.push_back("vertex_alpha");
    }
    if (!m_face_colors.empty()) {
        names.push_back("face_red");
        names.push_back("face_green");
        names.push_back("face_blue");
        names.push_back("face_alpha");
    }
    return names;
}

size_t OFFParser::get_attribute_size(const std::string& name) const {
    if (name.at(0) == 'v') return m_num_vertices;
    else if (name.at(0) == 'f') return m_num_faces;
    else {
        std::stringstream err_msg;
        err_msg << "Attribute " << name << " does not exists";
        throw IOError(err_msg.str());
    }
}

void OFFParser::export_vertices(Float* buffer) {
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

void OFFParser::export_faces(int* buffer) {
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

void OFFParser::export_attribute(const std::string& name, Float* buffer) {
    if (name.substr(0, 6) == "vertex") {
        export_color(m_vertex_colors, name.substr(7), buffer);
    } else if (name.substr(0, 4) == "face") {
        export_color(m_face_colors, name.substr(5), buffer);
    } else {
        std::stringstream err_msg;
        err_msg << "Unknown attribute name: " << name << std::endl;
        throw IOError(err_msg.str());
    }
}

void OFFParser::check_header(char* line) {
    if (std::string(line).substr(0, 3) != "OFF" &&
            std::string(line).substr(0, 4) != "COFF" &&
            std::string(line).substr(0, 4) != "NOFF" &&
            std::string(line).substr(0, 5) != "STOFF") {
        std::stringstream err_msg;
        err_msg << "Incorrect OFF header: " << line;
        throw IOError(err_msg.str());
    }

    if (std::string(line).substr(0, 10) == "OFF BINARY") {
        throw NotImplementedError("Binary OFF format is not supported.");
    }
}

void OFFParser::parse_geometry_counts(char* line) {
    size_t num_edges;
    size_t n = sscanf(line, "%zi %zi %zi", &m_num_vertices, &m_num_faces, &num_edges);
    if (n != 3) {
        throw IOError("Unable to parser geometry counts");
    }
}

void OFFParser::parse_vertex_line(char* line) {
    Float data[7];
    size_t n = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf",
            &data[0], &data[1], &data[2],
            &data[3], &data[4], &data[5], &data[6]);
    if (n >= 3) {
        m_vertices.push_back(Vector3F(data[0], data[1], data[2]));
    } else {
        throw IOError("Error parsing OFF file vertex line.");
    }

    if (n == 7) {
        m_vertex_colors.push_back(Vector4F(data[3], data[4], data[5], data[6]));
    }
}

void OFFParser::parse_face_line(char* line) {
    const char WHITE_SPACE[] = " \t\n\r";
    char* field = strtok(line, WHITE_SPACE);
    assert(field != NULL);

    const size_t n = atoi(field);
    VectorI face(n);
    for (size_t i=0; i<n; i++) {
        field = strtok(NULL, WHITE_SPACE);
        if (field == NULL) {
            throw IOError("Error parsing faces.");
        }
        face[i] = atoi(field);
    }
    if (n == 3) m_tris.push_back(face);
    else if (n == 4) m_quads.push_back(face);
    else if (n > 4){
        std::cerr << "Converting " << n << "-gon to triangles" << std::endl;
        for (size_t i=0; i<n-1; i++) {
            VectorI f(3);
            f << face[0], face[i], face[i+1];
            m_tris.push_back(f);
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Invalid polygon with " << n << "sides.";
        throw IOError(err_msg.str());
    }

    //bool with_color = true;
    //Vector4F color;
    //for (size_t i=0; i<4; i++) {
    //    field = strtok(NULL, WHITE_SPACE);
    //    if (field == NULL) {
    //        with_color = false;
    //        break;
    //    }
    //    color[i] = atof(field);
    //}
    //if (with_color) {
    //    m_face_colors.push_back(color);
    //}
}

void OFFParser::unify_faces() {
    if (m_tris.size() > 0 && m_quads.size() == 0) {
        m_faces = std::move(m_tris);
        m_vertex_per_face = 3;
    } else if (m_tris.size() == 0 && m_quads.size() > 0) {
        m_faces = std::move(m_quads);
        m_vertex_per_face = 4;
    } else if (m_tris.size() > 0 && m_quads.size() > 0){
        std::cerr << "Mixed triangle and quads in the input file" << std::endl;
        std::cerr << "Converting quads in triangles, face order is not kept!"
            << std::endl;
        m_faces = std::move(m_tris);
        for (auto& quad : m_quads) {
            m_faces.push_back(Vector3I(quad[0], quad[1], quad[2]));
            m_faces.push_back(Vector3I(quad[0], quad[2], quad[3]));
        }
        m_vertex_per_face = 3;
    }
}

void OFFParser::finalize_colors() {
    if (!m_vertex_colors.empty() && m_vertex_colors.size() != m_vertices.size()) {
        std::cerr << "Num vertex colors does not match num vertices.  Ignoring colors."
            << std::endl;
        m_vertex_colors.clear();
    }
    if (!m_face_colors.empty() && m_face_colors.size() != m_faces.size()) {
        std::cerr << "Num face colors does not match num faces.  Ignoring colors."
            << std::endl;
        m_face_colors.clear();
    }
}

void OFFParser::export_color(const ColorList& colors, const std::string& name,
        Float* buffer) {
    size_t count = 0;
    size_t color_index = lookup_color_index(name);
    for (const auto& color : colors) {
        buffer[count] = color[color_index];
        count++;
    }
}

