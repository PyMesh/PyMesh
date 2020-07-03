/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include "Grid.h"

namespace PyMesh {

template<int DIM>
class Eroder {
    public:
        typedef Grid<DIM, bool> Mask;
        typedef typename Mask::Vector_i Vector_i;

    public:
        Eroder(Mask& mask) : m_mask(mask) {}
        virtual ~Eroder() {}

    public:
        void erode(size_t iterations);
        Mask get_mask() const { return m_mask; }

    protected:
        Mask m_mask;
};

}

#include "Eroder.inl"
