#include "WireEdgeSymmetryAttribute.h"

#include <vector>
#include <list>

#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Misc/SymmetryOperators.h>

namespace WireEdgeSymmetryAttributeHelper {
    using namespace SymmetryOperators;

    Operators get_symmetry_operators(size_t dim, const VectorF& center) {
        if (dim == 2) {
            return generate_symmetry_operators<2>(center);
        } else if (dim == 3) {
            return generate_symmetry_operators<3>(center);
        } else {
            throw NotImplementedError(
                    "Symmetry operations only supports 2D and 3D");
        }
    }

    std::vector<std::list<size_t> > compute_connectivity(
            const MatrixFr& vertices, const MatrixIr& edges, Operators& ops) {
        const Float tol = 1e-12;
        const size_t num_edges = edges.rows();

        std::vector<std::list<size_t> > connectivity(num_edges);
        for (auto& op : ops) {
            std::vector<bool> visited(num_edges, false);
            for (size_t i=0; i<num_edges; i++) {
                if (visited[i]) continue;

                const VectorI& curr_e = edges.row(i);
                const VectorF& e0 = vertices.row(curr_e[0]);
                const VectorF& e1 = vertices.row(curr_e[1]);
                VectorF mapped_e0 = op(e0);
                VectorF mapped_e1 = op(e1);

                for (size_t j=i+1; j<num_edges; j++) {
                    const VectorI& other_e = edges.row(j);
                    const VectorF& other_e0 = vertices.row(other_e[0]);
                    const VectorF& other_e1 = vertices.row(other_e[1]);

                    Float dist_1 = (mapped_e0 - other_e0).squaredNorm() +
                        (mapped_e1 - other_e1).squaredNorm();
                    Float dist_2 = (mapped_e0 - other_e1).squaredNorm() +
                        (mapped_e1 - other_e0).squaredNorm();
                    if (dist_1 < tol || dist_2 < tol) {
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

    VectorI get_edge_labels(const MatrixFr& vertices, const MatrixIr& edges, Operators& ops) {
        std::vector<std::list<size_t> > connectivity =
            compute_connectivity(vertices, edges, ops);
        return label_connected_components(edges.rows(), connectivity);
    }
}
using namespace WireEdgeSymmetryAttributeHelper;

void WireEdgeSymmetryAttribute::compute(const WireNetwork& network) {
    Operators ops = get_symmetry_operators(
            network.get_dim(), network.center());
    VectorI orbit_labels = get_edge_labels(
            network.get_vertices(), network.get_edges(), ops);
    m_values = orbit_labels.cast<Float>();
}
