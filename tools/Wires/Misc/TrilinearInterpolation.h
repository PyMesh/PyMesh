/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Interpolation.h"

namespace PyMesh {

class TrilinearInterpolation : public Interpolation {
    public:
        TrilinearInterpolation(const MatrixFr& control_pts);
        virtual ~TrilinearInterpolation() {}

    public:
        VectorF interpolate(const VectorF& p) const;
        MatrixFr interpolate_batch(const MatrixFr& pts) const;

        static const Functions m_shape_functions;
};

}
