/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Interpolation.h"

namespace PyMesh {

class BilinearInterpolation : public Interpolation {
    public:
        BilinearInterpolation(const MatrixFr& control_pts);
        virtual ~BilinearInterpolation() {}

    public:
        VectorF interpolate(const VectorF& p) const;
        MatrixFr interpolate_batch(const MatrixFr& pts) const;

        static const Functions m_shape_functions;
};

}
