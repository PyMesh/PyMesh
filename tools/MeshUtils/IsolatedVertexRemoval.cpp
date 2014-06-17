#include "IsolatedVertexRemoval.h"
#include <cassert>
#include <vector>

IsolatedVertexRemoval::IsolatedVertexRemoval(
        MatrixFr& vertices, MatrixIr& faces) :
    m_vertices(vertices), m_faces(faces) { }

size_t IsolatedVertexRemoval::run() {
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    const size_t dim = m_vertices.cols();
    const size_t vertex_per_face = m_faces.cols();

    std::vector<bool> is_isolated(num_vertices, true);
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            is_isolated[m_faces.coeff(i,j)] = false;
        }
    }

    size_t count = 0;
    VectorI index_map = VectorI::Ones(num_vertices) * -1;
    for (size_t i=0; i<num_vertices; i++) {
        if (is_isolated[i]) continue;
        index_map[i] = count;
        count++;
    }

    MatrixFr vertices(count, dim);
    for (size_t i=0; i<num_vertices; i++) {
        if (is_isolated[i]) continue;
        vertices.row(index_map[i]) = m_vertices.row(i);
    }

    m_vertices = vertices;
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            assert(!is_isolated[m_faces.coeff(i,j)]);
            m_faces.coeffRef(i,j) = index_map[m_faces.coeff(i,j)];
        }
    }

    return num_vertices - count;
}
