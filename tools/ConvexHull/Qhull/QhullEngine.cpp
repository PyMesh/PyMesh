/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "QhullEngine.h"

#include <iostream>
#include <sstream>
#include <list>
#include <vector>

#include <Core/Exception.h>

#ifdef __INTEL_COMPILER
#include <qhull_a.h>
#else
extern "C" {
#include <qhull_a.h>
}
#endif

using namespace PyMesh;

void QhullEngine::run(const MatrixFr& points) {
    const size_t num_points = points.rows();
    const size_t dim = points.cols();
    char flags[64];
    sprintf(flags, "qhull Qt");
    coordT* data = new coordT[num_points * dim];
    std::copy(points.data(), points.data() + num_points * dim, data);

    m_lock.lock();
    int err = qh_new_qhull(dim, num_points, data, false,
            flags, NULL, stderr);

    if (!err) {
        extract_hull(points);
    } else {
        std::stringstream err_msg;
        err_msg << "Qhull error: " << err;
        throw RuntimeError(err_msg.str());
    }

    qh_freeqhull(!qh_ALL);
    m_lock.unlock();
    delete [] data;
    reorient_faces();
}

void QhullEngine::extract_hull(const MatrixFr& points) {
    const size_t dim = points.cols();
    const size_t num_input_points = points.rows();
    const size_t num_faces = qh num_facets;
    const size_t num_vertices = qh num_vertices;

    size_t index = 0;
    m_vertices.resize(num_vertices, dim);
    m_index_map.resize(num_vertices);
    VectorF inverse_map = VectorF::Ones(num_input_points) * -1;
    vertexT* vertex, **vertexp;
    FORALLvertices {
        size_t i = qh_pointid(vertex->point);
        m_vertices.row(index) = Eigen::Map<VectorF>(vertex->point, dim);
        m_index_map[index] = i;
        inverse_map[i] = index;
        index++;
    }

    index = 0;
    m_faces.resize(num_faces, dim);
    facetT *facet;
    FORALLfacets {
        size_t i=0;
        FOREACHvertex_( facet->vertices ) {
            assert(inverse_map(qh_pointid(vertex->point)) != -1);
            m_faces(index, i) = inverse_map(qh_pointid(vertex->point));
            i++;
        }
        index++;
    }
}

