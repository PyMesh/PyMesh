/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_FAST_WINDING_NUMBER

#include "FastWindingNumberEngine.h"

#include <cmath>
#include <vector>

#include <UT_SolidAngle.h>

using namespace PyMesh;

VectorF FastWindingNumberEngine::run(const MatrixFr& queries) {
    const auto num_vertices = m_vertices.rows();
    const auto num_faces = m_faces.rows();
    const auto num_queries = queries.rows();
    using Vector = HDK_Sample::UT_Vector3T<float>;
    using Engine = HDK_Sample::UT_SolidAngle<float, float>;

    std::vector<Vector> vertices(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        vertices[i][0] = static_cast<float>(m_vertices(i, 0));
        vertices[i][1] = static_cast<float>(m_vertices(i, 1));
        vertices[i][2] = static_cast<float>(m_vertices(i, 2));
    }

    Engine engine;
    engine.init(num_faces, m_faces.data(), num_vertices, vertices.data());

    VectorF winding_numbers(num_queries);
    for (size_t i=0; i<num_queries; i++) {
        Vector q;
        q[0] = static_cast<float>(queries(i, 0));
        q[1] = static_cast<float>(queries(i, 1));
        q[2] = static_cast<float>(queries(i, 2));
        winding_numbers[i] = engine.computeSolidAngle(q);
    }
    winding_numbers /= 4 * M_PI;

    return winding_numbers;
}

#endif
