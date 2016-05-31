/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TrilinearInterpolation.h"

#include <sstream>
#include <Core/Exception.h>

using namespace PyMesh;

// Shape function order according to hex vertex order:
//
//   3 ________ 2
//    /:      /| 
// 7 / :   6 / |
//  +-------+  |
//  | 0: ...|..| 1
//  | ;     | /
//  |;      |/
//  +-------+
//  4        5

const TrilinearInterpolation::Functions TrilinearInterpolation::m_shape_functions = {
    [](const Vector3F& v) { return (1.0-v[0])*(1.0-v[1])*(1.0-v[2]); },
    [](const Vector3F& v) { return (    v[0])*(1.0-v[1])*(1.0-v[2]); },
    [](const Vector3F& v) { return (    v[0])*(    v[1])*(1.0-v[2]); },
    [](const Vector3F& v) { return (1.0-v[0])*(    v[1])*(1.0-v[2]); },
    [](const Vector3F& v) { return (1.0-v[0])*(1.0-v[1])*(    v[2]); },
    [](const Vector3F& v) { return (    v[0])*(1.0-v[1])*(    v[2]); },
    [](const Vector3F& v) { return (    v[0])*(    v[1])*(    v[2]); },
    [](const Vector3F& v) { return (1.0-v[0])*(    v[1])*(    v[2]); }
};

TrilinearInterpolation::TrilinearInterpolation(
        const MatrixFr& control_pts) : Interpolation(control_pts) {
    const size_t dim = m_control_pts.cols();
    const size_t num_pts = m_control_pts.rows();

    if (dim != 3) {
        std::stringstream err_msg;
        err_msg << "Invalid dim: " << dim
            << ", trilinear interpolation requires dim=3";
        throw RuntimeError(err_msg.str());
    }

    if (num_pts != 8) {
        std::stringstream err_msg;
        err_msg << "Given " << num_pts << " control points.  "
            << "Trilinear interpolation requires 8 control points.";
        throw RuntimeError(err_msg.str());
    }
}

VectorF TrilinearInterpolation::interpolate(const VectorF& p) const {
    VectorF trilinear_coordinates(8);
    trilinear_coordinates <<
        m_shape_functions[0](p),
        m_shape_functions[1](p),
        m_shape_functions[2](p),
        m_shape_functions[3](p),
        m_shape_functions[4](p),
        m_shape_functions[5](p),
        m_shape_functions[6](p),
        m_shape_functions[7](p);
    return (m_control_pts.transpose() * trilinear_coordinates).transpose();
}

MatrixFr TrilinearInterpolation::interpolate_batch(const MatrixFr& pts) const {
    const size_t num_pts = pts.rows();
    MatrixFr bilinear_coordinates(num_pts, 8);
    for (size_t i=0; i<num_pts; i++) {
        const VectorF& p = pts.row(i);
        bilinear_coordinates.row(i) << 
            m_shape_functions[0](p),
            m_shape_functions[1](p),
            m_shape_functions[2](p),
            m_shape_functions[3](p),
            m_shape_functions[4](p),
            m_shape_functions[5](p),
            m_shape_functions[6](p),
            m_shape_functions[7](p);
    }

    return bilinear_coordinates * m_control_pts;
}
