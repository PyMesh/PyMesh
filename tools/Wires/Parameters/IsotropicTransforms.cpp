/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IsotropicTransforms.h"

#include <cassert>
#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

MatrixF IsotropicTransforms::fit(
        const VectorF& from_dir, const VectorF& to_dir) const {
    const Float tol = 1e-12;

    for (const auto& rot1 : m_rotations) {
        for (const auto& ref : m_reflections) {
            for (const auto& rot2 : m_rotations) {
                VectorF dir = rot1 * ref * rot2 * from_dir;
                Float dist = (dir - to_dir).squaredNorm();
                if (dist < tol) {
                    return rot1 * ref * rot2;
                }
            }
        }
    }
    std::stringstream err_msg;
    err_msg << "Cannot find isotropic transformation that maps <"
        << from_dir.transpose() << "> to <"
        << to_dir.transpose() << ">" << std::endl;
    throw RuntimeError(err_msg.str());
}

void IsotropicTransforms::initialize() {
    if (m_dim == 2) {
        initialize_2D_reflections();
        initialize_2D_rotations();
    } else if (m_dim == 3) {
        initialize_3D_reflections();
        initialize_3D_rotations();
    } else {
        throw NotImplementedError("Only 2D and 3D is supported");
    }
}

void IsotropicTransforms::initialize_2D_reflections() {
    m_reflections.clear();
    m_reflections.push_back(MatrixF::Identity(2, 2));
    Matrix2F ref_x, ref_y, ref_xy;
    ref_x << -1, 0, 0,  1;
    ref_y <<  1, 0, 0, -1;
    ref_xy<< -1, 0, 0, -1;
    m_reflections.push_back(ref_x);
    m_reflections.push_back(ref_y);
    m_reflections.push_back(ref_xy);
}

void IsotropicTransforms::initialize_2D_rotations() {
    m_rotations.clear();
    m_rotations.push_back(MatrixF::Identity(2, 2));
    Matrix2F rot_90, rot_180, rot_270;
    rot_90  <<  0, -1,  1,  0;
    rot_180 << -1,  0,  0, -1;
    rot_270 <<  0,  1, -1,  0;
    m_rotations.push_back(rot_90);
    m_rotations.push_back(rot_180);
    m_rotations.push_back(rot_270);
}

void IsotropicTransforms::initialize_3D_reflections() {
    m_reflections.clear();
    m_reflections.push_back(MatrixF::Identity(3, 3));
    Matrix3F ref_x, ref_y, ref_z;
    Matrix3F ref_xy, ref_yz, ref_zx;
    Matrix3F ref_xyz;
    ref_x   << -1, 0, 0, 0, 1, 0, 0, 0, 1;
    ref_y   <<  1, 0, 0, 0,-1, 0, 0, 0, 1;
    ref_z   <<  1, 0, 0, 0, 1, 0, 0, 0,-1;
    ref_xy  << -1, 0, 0, 0,-1, 0, 0, 0, 1;
    ref_yz  <<  1, 0, 0, 0,-1, 0, 0, 0,-1;
    ref_zx  << -1, 0, 0, 0, 1, 0, 0, 0,-1;
    ref_xyz << -1, 0, 0, 0,-1, 0, 0, 0,-1;
    m_reflections.push_back(ref_x);
    m_reflections.push_back(ref_y);
    m_reflections.push_back(ref_z);
    m_reflections.push_back(ref_xy);
    m_reflections.push_back(ref_yz);
    m_reflections.push_back(ref_zx);
    m_reflections.push_back(ref_xyz);
}

void IsotropicTransforms::initialize_3D_rotations() {
    m_rotations.clear();
    m_rotations.push_back(MatrixF::Identity(3, 3));
    Matrix3F rot_x_90, rot_x_180, rot_x_270;
    Matrix3F rot_y_90, rot_y_180, rot_y_270;
    Matrix3F rot_z_90, rot_z_180, rot_z_270;

    rot_x_90 << 1, 0, 0, 0, 0, -1, 0, 1, 0;
    rot_x_180 = rot_x_90 * rot_x_90;
    rot_x_270 = rot_x_180 * rot_x_90;

    rot_y_90 << 0, 0, -1, 0, 1, 0, 1, 0, 0;
    rot_y_180 = rot_y_90 * rot_y_90;
    rot_y_270 = rot_y_180 * rot_y_90;

    rot_z_90 << 0, -1, 0, 1, 0, 0, 0, 0, 1;
    rot_z_180 = rot_z_90 * rot_z_90;
    rot_z_270 = rot_z_180 * rot_z_90;

    m_rotations.push_back(rot_x_90);
    m_rotations.push_back(rot_x_180);
    m_rotations.push_back(rot_x_270);
    m_rotations.push_back(rot_y_90);
    m_rotations.push_back(rot_y_180);
    m_rotations.push_back(rot_y_270);
    m_rotations.push_back(rot_z_90);
    m_rotations.push_back(rot_z_180);
    m_rotations.push_back(rot_z_270);
}

