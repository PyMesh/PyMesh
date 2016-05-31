/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IsotropicDofExtractor.h"

#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

IsotropicDofExtractor::IsotropicDofExtractor(WireNetwork::Ptr wire_network) :
    m_wire_network(wire_network) {
        m_bbox_min = wire_network->get_bbox_min();
        m_bbox_max = wire_network->get_bbox_max();
        m_bbox_center = wire_network->center();
        m_bbox_half_size = 0.5 * (m_bbox_max[0] - m_bbox_min[0]);

        check_bbox_is_isotropic();
    }

//std::vector<VectorF> IsotropicDofExtractor::extract_dofs(const VectorI& orbit) {
//    check_orbit_cardinality(orbit.size());
//
//    const size_t dim = m_wire_network->get_dim();
//    if (dim == 3) {
//        return extract_3D_dofs(orbit);
//    } else if (dim == 2) {
//        return extract_2D_dofs(orbit);
//    } else {
//        std::stringstream err_msg;
//        err_msg << "Unsupported dim: " << dim;
//        throw NotImplementedError(err_msg.str());
//    }
//}

std::vector<VectorF> IsotropicDofExtractor::extract_dofs(const VectorF& v) {
    const size_t dim = m_wire_network->get_dim();
    assert(v.size() == dim);
    if (dim == 3) {
        return extract_3D_dofs(v);
    } else if (dim == 2) {
        return extract_2D_dofs(v);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

void IsotropicDofExtractor::check_bbox_is_isotropic() const {
    VectorF half_size = 0.5 * (m_bbox_max - m_bbox_min);
    VectorF gap = (half_size.array() - m_bbox_half_size).cwiseAbs();
    if (gap.minCoeff() > 1e-12) {
        throw RuntimeError("BBox is not isotropic.");
    }
}

//void IsotropicDofExtractor::check_orbit_cardinality(size_t orbit_size) const {
//    const size_t dim = m_wire_network->get_dim();
//    if (dim == 3) {
//        if (orbit_size != 48 &&
//                orbit_size != 24 &&
//                orbit_size != 12 &&
//                orbit_size != 8 &&
//                orbit_size != 6 &&
//                orbit_size != 1) {
//            std::stringstream err_msg;
//            err_msg << "Invalid orbit cardinality: " << orbit_size << std::endl;
//            err_msg << "Valid 3D isotropic orbit sizes are 48, 24, 12, 8, 6, 1";
//            throw RuntimeError(err_msg.str());
//        }
//    } else if (dim == 2) {
//        if (orbit_size != 8 &&
//                orbit_size != 4 &&
//                orbit_size != 1) {
//            std::stringstream err_msg;
//            err_msg << "Invalid orbit cardinality: " << orbit_size << std::endl;
//            err_msg << "Valid 2D isotropic orbit sizes are 8, 4, 1";
//            throw RuntimeError(err_msg.str());
//        }
//    } else {
//        std::stringstream err_msg;
//        err_msg << "Unsupported dim: " << dim;
//        throw NotImplementedError(err_msg.str());
//    }
//}

std::vector<VectorF> IsotropicDofExtractor::extract_3D_dofs(
        const VectorF& v) {
    const Float tol = 1e-12;
    const VectorF dir = v - m_bbox_center;

    std::vector<VectorF> dofs;

    auto comp = [=](size_t i1, size_t i2) {
        return fabs(dir[i1]) + tol < fabs(dir[i2]);
    };
    std::vector<size_t> order = {0, 1, 2};
    std::sort(order.begin(), order.end(), comp);

    std::vector<bool> processed = {false, false, false};
    for (size_t i=0; i<3; i++) {
        if (processed[order[i]]) continue;
        processed[order[i]] = true;

        Float val = dir[order[i]];
        if (fabs(val) < tol) continue;
        if (fabs(fabs(val) - m_bbox_half_size) < tol) continue;

        Vector3F dof(0, 0, 0);
        dof[order[i]] = val > 0 ? 1.0 : -1.0;

        for (size_t j=i+1; j<3; j++) {
            if (!comp(order[i], order[j]) &&
                    !comp(order[j], order[i])) {
                processed[order[j]] = true;
                Float val_j = dir[order[j]];
                dof[order[j]] = val_j > 0 ? 1.0 : -1.0;
            }
        }
        dofs.push_back(dof);
    }
    return dofs;
}

std::vector<VectorF> IsotropicDofExtractor::extract_2D_dofs(
        const VectorF& v) {
    const Float tol = 1e-12;
    const VectorF dir = v - m_bbox_center;

    std::vector<VectorF> dofs;
    if (fabs(dir[0]) < tol && fabs(dir[1]) < tol) { return dofs; }
    bool on_center[] = {
        fabs(dir[0]) < tol,
        fabs(dir[1]) < tol
    };
    bool on_border[] = {
        fabs(fabs(dir[0]) - m_bbox_half_size) < tol,
        fabs(fabs(dir[1]) - m_bbox_half_size) < tol
    };

    bool on_diagonal = fabs(fabs(dir[0]) - fabs(dir[1])) < tol;
    if (on_diagonal) {
        if (!on_center[0] && !on_border[0]) {
            assert(!on_center[1]);
            assert(!on_border[1]);
            Vector2F dof(0, 0);
            dof[0] = dir[0] > 0 ? 1.0 : -1.0;
            dof[1] = dir[1] > 0 ? 1.0 : -1.0;
            dofs.push_back(dof);
        }
    } else {
        if (!on_center[0] && !on_border[0]) {
            Vector2F dof(0, 0);
            dof[0] = dir[0] > 0 ? 1.0 : -1.0;
            dofs.push_back(dof);
        }
        if (!on_center[1] && !on_border[1]) {
            Vector2F dof(0, 0);
            dof[1] = dir[1] > 0 ? 1.0 : -1.0;
            dofs.push_back(dof);
        }
    }

    return dofs;
}

