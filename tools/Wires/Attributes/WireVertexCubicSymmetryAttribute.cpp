/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireVertexCubicSymmetryAttribute.h"

#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Misc/SymmetryOperators.h>

using namespace PyMesh;

namespace WireVertexCubicSymmetryAttributeHelper {
    using namespace SymmetryOperators;

    Operators get_symmetry_operators(size_t dim, const VectorF& center) {
        Operators ops;
        if (dim == 2) {
            Operators reflective_ops = generate_reflective_symmetry_operators<2>(center);
            Operators rotational_ops = generate_rotational_symmetry_operators<2>(center);
            ops.insert(ops.end(), reflective_ops.begin(), reflective_ops.end());
            ops.insert(ops.end(), rotational_ops.begin(), rotational_ops.end());
        } else if (dim == 3) {
            Operators reflective_ops = generate_reflective_symmetry_operators<3>(center);
            Operators rotational_ops = generate_rotational_symmetry_operators<3>(center);
            ops.insert(ops.end(), reflective_ops.begin(), reflective_ops.end());
            ops.insert(ops.end(), rotational_ops.begin(), rotational_ops.end());
        } else {
            throw NotImplementedError(
                    "Symmetry operations only supports 2D and 3D");
        }
        return ops;
    }

    VectorI get_vertex_labels(const MatrixFr& vertices, Operators& ops) {
        SymmetryConnectivity connectivity =
            compute_vertex_connectivity(vertices, ops);
        return label_connected_components(vertices.rows(), connectivity);
    }
}
using namespace WireVertexCubicSymmetryAttributeHelper;

void WireVertexCubicSymmetryAttribute::compute(const WireNetwork& network) {
    Operators ops = get_symmetry_operators(
            network.get_dim(), network.center());
    VectorI orbit_labels = get_vertex_labels(network.get_vertices(), ops);
    m_values = orbit_labels.cast<Float>();
}
