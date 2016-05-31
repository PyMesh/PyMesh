/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Boolean/BooleanEngine.h>
#include <clipper.hpp>

namespace PyMesh {

class ClipperEngine : public BooleanEngine {
    public:
        virtual ~ClipperEngine() {}

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();

    protected:
        void clip(ClipperLib::ClipType type);
        void initialize_if_needed();
        void extract_boundary_loops();
        void extract_result(const ClipperLib::Paths& paths);

    protected:
        ClipperLib::Paths m_loops_1;
        ClipperLib::Paths m_loops_2;
};

}
