/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireVertexSymmetryAttribute.h"

#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Misc/SymmetryOperators.h>

using namespace PyMesh;

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

    VectorI get_vertex_labels(const MatrixFr& vertices, Operators& ops) {
        SymmetryConnectivity connectivity =
            compute_vertex_connectivity(vertices, ops);
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

