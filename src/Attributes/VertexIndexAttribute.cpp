#include "VertexIndexAttribute.h"

#include <Mesh.h>

void VertexIndexAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_vertices = mesh.get_num_vertices();
    VectorF& indices = m_values;
    indices = VectorF::LinSpaced(num_vertices, 0, num_vertices-1);
}
