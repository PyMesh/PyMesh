/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireWriter.h"

#include <Core/Exception.h>

#include <cassert>
#include <fstream>
#include <sstream>

using namespace PyMesh;

void WireWriter::write(const std::string& filename, const WireNetwork& wires) {
    const MatrixFr& vertices = wires.get_vertices();
    const MatrixIr& edges = wires.get_edges();
    write_raw(filename, vertices, edges);
}

void WireWriter::write_raw(const std::string& filename,
        const MatrixFr& vertices, const MatrixIr& edges) {
    const size_t num_vertices = vertices.rows();
    const size_t dim = vertices.cols();
    const size_t num_edges = edges.rows();
    assert(edges.cols() == 2);

    std::ofstream fout(filename.c_str());
    if (!fout.is_open()) {
        std::stringstream err_msg;
        err_msg << "Unable to open file \"" << filename << "\" for writing";
        throw IOError(err_msg.str());
    }

    for (size_t i=0; i<num_vertices; i++) {
        fout << "v";
        for (size_t j=0; j<dim; j++) {
            fout << " " << std::fixed << vertices(i,j);
        }
        fout << std::endl;
    }

    for (size_t i=0; i<num_edges; i++) {
        fout << "l " << edges(i, 0)+1 << " " << edges(i, 1)+1 << std::endl;
    }

    fout.close();
}
