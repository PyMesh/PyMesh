/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL_AND_CGAL

#include <OuterHull/OuterHullEngine.h>

namespace PyMesh {

class IGLOuterHullEngine : public OuterHullEngine {
    public:
        virtual ~IGLOuterHullEngine() = default;

    public:
        virtual void run();

    private:
        void extract_outer_hull();

    private:
        typedef Eigen::Matrix<Float, Eigen::Dynamic, 3, Eigen::RowMajor> Matrix3Fr;
};

}

#endif
