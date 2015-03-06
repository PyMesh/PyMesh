#pragma once

#include <Boolean/OuterHullEngine.h>

class IGLOuterHullEngine : public OuterHullEngine {
    public:
        virtual ~IGLOuterHullEngine() {}

    public:
        virtual void run();

    private:
        void extract_face_normals();
        void check_normal_reliability();
        void resolve_self_intersections();
        void extract_outer_hull();

    private:
        typedef Eigen::Matrix<Float, Eigen::Dynamic, 3, Eigen::RowMajor> Matrix3Fr;
        Matrix3Fr m_normals;
};
