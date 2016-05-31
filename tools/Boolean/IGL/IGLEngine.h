/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Boolean/BooleanEngine.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

namespace PyMesh {

class IGLEngine : public BooleanEngine {
    public:
        virtual ~IGLEngine() {}

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();

        virtual VectorI get_face_sources() const {
            return m_face_sources;
        }

        virtual void serialize_xml(const std::string& filename) const;

    public:
        typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
        typedef Eigen::Matrix<
            Kernel::FT,
            Eigen::Dynamic,
            Eigen::Dynamic,
            Eigen::RowMajor> MatrixEr;

    private:
        VectorI m_face_sources;
        MatrixEr m_exact_vertices;
};

}
