#pragma once

#include "Interpolation.h"

class BilinearInterpolation : public Interpolation {
    public:
        BilinearInterpolation(const MatrixFr& control_pts);
        virtual ~BilinearInterpolation() {}

    public:
        VectorF interpolate(const VectorF& p) const;
        MatrixFr interpolate_batch(const MatrixFr& pts) const;

        static const Functions m_shape_functions;
};
