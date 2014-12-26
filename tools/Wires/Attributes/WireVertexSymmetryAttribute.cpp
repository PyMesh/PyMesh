#include "WireVertexSymmetryAttribute.h"

#include <vector>
#include <list>

#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Misc/SymmetryOperators.h>

namespace WireVertexSymmetryAttributeHelper {
    using namespace SymmetryOperators;

    Operators get_symmetry_operators(size_t dim, const VectorF& center) {
        if (dim == 2) {
            return generate_reflective_symmetry_operators<2>(center);
        } else if (dim == 3) {
            return generate_reflective_symmetry_operators<3>(center);
        } else {
            throw NotImplementedError(
                    "Symmetry operations only supports 2D and 3D");
        }
    }

    std::vector<std::list<size_t> > compute_connectivity(
            const MatrixFr& vertices, Operators& ops) {
        const Float tol = 1e-12;
        const size_t num_vertices = vertices.rows();

        std::vector<std::list<size_t> > connectivity(num_vertices);
        for (auto& op : ops) {
            std::vector<bool> visited(num_vertices, false);
            for (size_t i=0; i<num_vertices; i++) {
                if (visited[i]) continue;

                const VectorF& curr_p = vertices.row(i);
                VectorF mapped_p = op(curr_p);
                for (size_t j=i+1; j<num_vertices; j++) {
                    const VectorF& other_p = vertices.row(j);
                    Float squared_dist = (mapped_p - other_p).squaredNorm();
                    if (squared_dist < tol) {
                        visited[i] = true;
                        visited[j] = true;
                        connectivity[i].push_back(j);
                        connectivity[j].push_back(i);
                    }
                }
            }
        }

        return connectivity;
    }

    VectorI get_vertex_labels(const MatrixFr& vertices, Operators& ops) {
        std::vector<std::list<size_t> > connectivity =
            compute_connectivity(vertices, ops);
        return label_connected_components(vertices.rows(), connectivity);
    }
}
using namespace WireVertexSymmetryAttributeHelper;

void WireVertexSymmetryAttribute::compute(const WireNetwork& network) {
    Operators ops = get_symmetry_operators(
            network.get_dim(), network.center());
    VectorI orbit_labels = get_vertex_labels(network.get_vertices(), ops);
    m_values = orbit_labels.cast<Float>();
}

