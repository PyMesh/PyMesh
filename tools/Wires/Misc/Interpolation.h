/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Core/EigenTypedef.h>

namespace PyMesh {
class Interpolation {
    public:
        typedef std::function<Float(const VectorF&)> Function;
        typedef std::vector<Function> Functions;

        Interpolation(const MatrixFr& control_pts) : m_control_pts(control_pts) {}
        virtual ~Interpolation() {}

    public:
        virtual VectorF interpolate(const VectorF& p) const=0;
        virtual MatrixFr interpolate_batch(const MatrixFr& pts) const=0;

    protected:
        MatrixFr m_control_pts;
};
}
