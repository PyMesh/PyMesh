/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireVertexMinAngleAttribute.h"

#include <cmath>

#include <Core/Exception.h>
#include <Wires/WireNetwork/WireNetwork.h>

using namespace PyMesh;

namespace WireVertexMinAngleAttributeHelper {
    template<size_t DIM>
    class Angle {
        public:
            typedef Eigen::Matrix<Float, DIM, 1> Vector;
            Float operator()(const Vector& e1, const Vector& e2) {
                std::stringstream err_msg;
                err_msg<< "Unsupported dim: " << DIM;
                throw NotImplementedError(err_msg.str());
            }
    };

    template<>
    class Angle<2> {
        public:
            typedef Eigen::Matrix<Float, 2, 1> Vector;
            Float operator()(const Vector& e1, const Vector& e2) {
                Float sin_val = std::abs(e1[0]*e2[1] - e1[1]*e2[0]);
                Float cos_val = e1.dot(e2);
                return atan2(sin_val, cos_val);
            }
    };

    template<>
    class Angle<3> {
        public:
            typedef Eigen::Matrix<Float, 3, 1> Vector;
            Float operator()(const Vector& e1, const Vector& e2) {
                Float sin_val = e1.cross(e2).norm();
                Float cos_val = e1.dot(e2);
                return atan2(sin_val, cos_val);
            }
    };

    template<typename T>
    void compute_min_angles(const WireNetwork& network, MatrixFr& values,
            T& angle) {
        const size_t num_vertices = network.get_num_vertices();

        // By default, min angle is the max angle possible: 180 degrees.
        values = MatrixFr::Ones(num_vertices, 1) * M_PI;
        const MatrixFr& vertices = network.get_vertices();
        for (size_t i=0; i<num_vertices; i++) {
            const VectorF& v = vertices.row(i);
            VectorI neighbors = network.get_vertex_neighbors(i);

            const size_t valance = neighbors.size();
            for (size_t j=0; j<valance; j++) {
                VectorF ej = vertices.row(neighbors[j]) - v.transpose();
                for (size_t k=j+1; k<valance; k++) {
                    VectorF ek = vertices.row(neighbors[k]) - v.transpose();
                    values.coeffRef(i,0) =
                        std::min(values.coeffRef(i,0), angle(ej, ek));
                }
            }
        }
    }
}

using namespace WireVertexMinAngleAttributeHelper;

void WireVertexMinAngleAttribute::compute(const WireNetwork& network) {
    if (!network.with_connectivity()) {
        throw RuntimeError("Min angle attribute requires wire connectivity.");
    }

    const size_t dim = network.get_dim();
    if (dim == 2) {
        Angle<2> angle;
        compute_min_angles(network, m_values, angle);
    } else if (dim == 3) {
        Angle<3> angle;
        compute_min_angles(network, m_values, angle);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}
