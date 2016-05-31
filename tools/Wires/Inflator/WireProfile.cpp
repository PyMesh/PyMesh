/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireProfile.h"
#include <Core/Exception.h>
#include <iostream>
#include <Eigen/Geometry>

using namespace PyMesh;

namespace WireProfileHelper {
    MatrixFr rotate_loop_2D(const MatrixFr& loop, const VectorF& dir) {
        const Float angle = atan2(-dir[0], dir[1]);
        Eigen::Rotation2D<Float> rotation(angle);
        return (rotation.toRotationMatrix() * loop.transpose()).transpose();
    }

    MatrixFr rotate_loop_3D(const MatrixFr& loop, const VectorF& dir) {
        const Vector3F X(1, 0, 0);
        const Vector3F Z(0, 0, 1);

        Eigen::Quaternion<Float> Q;
        Float dir_len = dir.norm();
        if (dir[2] < (-1.0 + 1e-6) * dir_len) {
            // Handle special case: dir ~= -Z
            // Eigen::Quaternion introduce large numeric error for such cases.
            Q = Eigen::Quaternion<Float>(
                    Eigen::AngleAxis<Float>(M_PI, Vector3F::UnitX()));
            Q = Eigen::Quaternion<Float>::FromTwoVectors(-Z, dir) * Q;
        } else {
            Q.setFromTwoVectors(Z, dir);
            if (fabs(dir[0]) > 1e-3 * dir_len && fabs(dir[1]) > 1e-3 * dir_len) {
                // Pre-rotate the profile so all wires has consistent profile
                // orientation.
                Q = Q * Eigen::Quaternion<Float>::FromTwoVectors(
                        X, Vector3F(dir[0], dir[1], 0.0));
            }
        }

        return (Q.toRotationMatrix() * loop.transpose()).transpose();
    }

    void apply_correction(const Vector3F& dir, MatrixFr& loop,
            const Vector3F& rel_correction, const Vector3F& abs_correction, 
            const Float correction_cap, const Float spread_const) {
        static const Float tol = 1e-3;
        assert(loop.cols() == abs_correction.size());
        assert(loop.cols() == rel_correction.size());
        assert(correction_cap >= 0.0);

        VectorF centroid = 0.5 *
            (loop.colwise().minCoeff() + loop.colwise().maxCoeff());
        const Vector3F upper_cap = Vector3F::Ones() * correction_cap * 0.5;
        const Vector3F lower_cap =-Vector3F::Ones() * correction_cap * 0.5;
        Float dir_sq_len = dir.squaredNorm();

        const size_t loop_size = loop.rows();
        if (fabs(dir[2]) > tol) {
            for (size_t i=0; i<loop_size; i++) {
                Vector3F v = loop.row(i) - centroid.transpose();
                Float factor = -v[2] / dir[2];
                Vector3F p = v + dir * factor;
                assert(fabs(p[2]) < 1e-12);

                Vector3F sign(1, 1, 1);
                if (v[0] < 0.0) sign[0] = -1;
                if (v[1] < 0.0) sign[1] = -1;
                if (v[2] < 0.0) sign[2] = -1;

                Float ave_XY_abs_offset = 0.5 * (abs_correction[0] +
                        abs_correction[1]);
                Vector3F offset(0, 0, 0);
                offset = p.cwiseProduct(rel_correction) +
                    Vector3F::UnitZ() * v[2] * rel_correction[2] +
                    0.5 * Vector3F::UnitZ() * sign[2] * abs_correction[2] +
                    0.5 * p / p.norm() * ave_XY_abs_offset;
                    //sign.cwiseProduct(abs_correction * 0.5);

                Float dist_to_center = p.norm();
                Float spread = 0.5 * spread_const;
                if (dist_to_center < 2.0 && 2.0 > ave_XY_abs_offset) {
                    spread = 0.5 * spread_const *
                        dist_to_center / (2.0 - ave_XY_abs_offset);
                }
                offset[0] += sign[0] * spread;
                offset[1] += sign[1] * spread;
                offset = offset.cwiseMin(upper_cap);
                offset = offset.cwiseMax(lower_cap);

                p += offset;
                p = p - dir * (dir.dot(p) / dir_sq_len);
                loop.row(i) = (p+centroid).transpose();
            }
        } else {
            for (size_t i=0; i<loop_size; i++) {
                Vector3F v = loop.row(i) - centroid.transpose();
                Vector3F p(v[0], v[1], 0.0);
                Vector3F sign(1, 1, 1);
                if (v[0] < 0.0) sign[0] = -1;
                if (v[1] < 0.0) sign[1] = -1;
                if (v[2] < 0.0) sign[2] = -1;

                Float ave_XY_abs_offset = 0.5 * (abs_correction[0] +
                        abs_correction[1]);
                Vector3F offset(0, 0, 0);
                offset = v.cwiseProduct(rel_correction) +
                    0.5 * Vector3F::UnitZ() * sign[2] * abs_correction[2] +
                    0.5 * p / p.norm() * ave_XY_abs_offset;
                    //sign.cwiseProduct(abs_correction * 0.5);
                Float spread = 0.5 * spread_const;
                offset[0] += sign[0] * spread;
                offset[1] += sign[1] * spread;

                offset = offset.cwiseMin(upper_cap);
                offset = offset.cwiseMax(lower_cap);

                v += offset;
                v = v - dir * (dir.dot(v) / dir_sq_len);
                loop.row(i) = (v+centroid).transpose();
            }
        }
    }
}

using namespace WireProfileHelper;

WireProfile::Ptr WireProfile::create(const std::string& name) {
    if (name == "square") {
        MatrixFr loop(4,3);
        loop << -1, -1, 0,
                 1, -1, 0,
                 1,  1, 0,
                -1,  1, 0;
        Ptr r = Ptr(new WireProfile());
        r->initialize(loop);
        return r;
    } else if (name == "hexagon") {
        return WireProfile::create_isotropic(6);
    } else if (name == "octagon") {
        return WireProfile::create_isotropic(8);
    } else if (name == "triangle") {
        return WireProfile::create_isotropic(3);
    } else {
        std::stringstream err_msg;
        err_msg << "Unknown loop profile: " << name;
        throw NotImplementedError(err_msg.str());
    }
}

WireProfile::Ptr WireProfile::create_isotropic(size_t num_samples) {
    MatrixFr loop = MatrixFr::Zero(num_samples, 3);
    for (size_t i=0; i<num_samples; i++) {
        Float angle = 2 * M_PI * i / num_samples;
        loop.coeffRef(i, 0) = cos(angle);
        loop.coeffRef(i, 1) = sin(angle);
    }
    Ptr r = Ptr(new WireProfile());
    r->initialize(loop);
    return r;
}

WireProfile::Ptr WireProfile::create_2D() {
    MatrixFr loop(2, 2);
    loop << -1, 0, 1, 0;
    Ptr r = Ptr(new WireProfile());
    r->initialize(loop);
    return r;
}

void WireProfile::initialize(const MatrixFr& loop) {
    m_loop = loop;
    m_dim = m_loop.cols();
    if (m_dim == 2) {
        m_offset_dir = Vector2F(0, 1);
    } else if (m_dim == 3) {
        m_offset_dir = Vector3F(0, 0, 1);
    }

    VectorF center = 0.5 * (m_loop.colwise().minCoeff() +
            m_loop.colwise().maxCoeff());
    m_loop.rowwise() -= center.transpose();
    m_radius = m_loop.rowwise().norm().maxCoeff();
}

MatrixFr WireProfile::place(const VectorF& end_1, const VectorF& end_2,
        Float offset, Float thickness,
        const VectorF& rel_correction,
        const VectorF& abs_correction,
        Float correction_cap,
        Float spread_const) {
    VectorF dir = end_2 - end_1;
    MatrixFr loop = m_loop;

    loop *= 0.5 * thickness / m_radius;
    loop.rowwise() += m_offset_dir.transpose() * offset;

    if (m_dim == 2) {
        loop = rotate_loop_2D(loop, dir);
    } else if (m_dim == 3) {
        loop = rotate_loop_3D(loop, dir);
        if (m_correction_table) {
            m_correction_table->apply_correction(dir, loop);
        }
    } else {
        assert(false);
        std::stringstream err_msg;
        err_msg << "Unsupported dimention: " << m_dim;
        throw NotImplementedError(err_msg.str());
    }
    return loop.rowwise() + end_1.transpose();
}

