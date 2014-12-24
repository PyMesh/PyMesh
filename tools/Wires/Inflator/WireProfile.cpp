#include "WireProfile.h"
#include <Core/Exception.h>
#include <iostream>
#include <Eigen/Geometry>

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
        Q.setFromTwoVectors(Z, dir);
        if (fabs(dir[0]) > 1e-6 || fabs(dir[1]) > 1e-6) {
            Q = Q * Eigen::Quaternion<Float>::FromTwoVectors(
                    X, Vector3F(dir[0], dir[1], 0.0));
        }
        return (Q.toRotationMatrix() * loop.transpose()).transpose();
    }

    void apply_correction(MatrixFr& loop, const VectorF& correction) {
        assert(loop.cols() == correction.size());
        VectorF centroid = 0.5 *
            (loop.colwise().minCoeff() + loop.colwise().maxCoeff());
        const size_t loop_size = loop.rows();
        for (size_t i=0; i<loop_size; i++) {
            VectorF v = loop.row(i) - centroid.transpose();
            Float l = v.norm();
            loop.row(i) += (v.cwiseProduct(correction) / l).transpose();
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
        Float offset, Float thickness, const VectorF& correction) {
    VectorF dir = end_2 - end_1;
    MatrixFr loop = m_loop;

    loop *= 0.5 * thickness / m_radius;
    loop.rowwise() += m_offset_dir.transpose() * offset;

    if (m_dim == 2) {
        loop = rotate_loop_2D(loop, dir);
    } else if (m_dim == 3) {
        loop = rotate_loop_3D(loop, dir);
    } else {
        assert(false);
        std::stringstream err_msg;
        err_msg << "Unsupported dimention: " << m_dim;
        throw NotImplementedError(err_msg.str());
    }
    apply_correction(loop, correction);
    return loop.rowwise() + end_1.transpose();
}

