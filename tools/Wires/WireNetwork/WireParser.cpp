/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireParser.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

WireParser::WireParser() : m_dim(0) { }

void WireParser::parse(const std::string& filename) {
    reset();
    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::stringstream err_msg;
        err_msg << "Failed to open " << filename;
        throw IOError(err_msg.str());
    }

    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];
    bool success;

    while (!fin.eof()) {
        fin.getline(line, LINE_SIZE);
        switch (line[0]) {
            case 'v':
                success = parse_vertex_line(line);
                break;
            case 'l':
                success = parse_edge_line(line);
                break;
            default:
                // Ignore other lines by default.
                success = true;
                break;
        }
        if (!success) {
            std::stringstream err_msg;
            err_msg << "Error parsing line: \"" << line << "\"";
            throw IOError(err_msg.str());
        }
    }
    fin.close();
}

void WireParser::export_vertices(Float* buffer) const {
    size_t i=0;
    for (auto v : m_vertices) {
        assert(v.size() == m_dim);
        std::copy(v.data(), v.data() + m_dim, buffer + i*m_dim);
        i++;
    }
}

void WireParser::export_edges(int* buffer) const {
    size_t i=0;
    for (auto e : m_edges) {
        assert(e.size() == 2);
        std::copy(e.data(), e.data() + 2, buffer + i*2);
        i++;
    }
}

void WireParser::reset() {
    m_dim = 0;
    m_vertices.clear();
    m_edges.clear();
}

bool WireParser::parse_vertex_line(char* line) {
    char header[8];
    Float data[4];
    size_t n = sscanf(line, "%s %lf %lf %lf %lf", header,
            &data[0], &data[1], &data[2], &data[3]);
    if (n < 3) return false;

    // Check to handle homogeneous coordinates.
    if (n == 5) {
        data[0] /= data[3];
        data[1] /= data[3];
        data[2] /= data[3];
        n -= 1;
    }
    if (m_dim == 0) { m_dim = n-1; }
    else if (m_dim != n-1) { return false; }

    Eigen::Map<VectorF> coord(data, m_dim);
    if (header[1] == '\0')
        m_vertices.push_back(coord);
    else if (header[1] == 'n') {
        // Not supported.
        std::cerr << "vertex normal is not supported" << std::endl;
    }
    return true;
}

bool WireParser::parse_edge_line(char* line) {
    char header[8];
    int data[2];
    size_t n = sscanf(line, "%s %i %i", header, &data[0], &data[1]);
    if (n != 3) { return false; }

    Eigen::Map<VectorI> edge(data, 2);
    edge.array() -= 1;
    m_edges.push_back(edge);
    return true;
}

