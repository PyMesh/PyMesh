#pragma once

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include "Grid.h"

template<int DIM>
class Eroder {
    public:
        typedef Grid<DIM, bool> Mask;
        typedef typename Mask::Vector_i Vector_i;

    public:
        Eroder(Mask& mask) : m_mask(mask) {}
        virtual ~Eroder() {}

    public:
        void erode(size_t iterations) {
            if (DIM == 2) {
                for (size_t i=0; i<iterations; i++) {
                    erode_2D();
                }
            } else if (DIM == 3) {
                for (size_t i=0; i<iterations; i++) {
                    erode_3D();
                }
            }
        }

        Mask get_mask() const { return m_mask; }

    protected:
        void erode_2D();
        void erode_3D();

    protected:
        Mask m_mask;
};

#include "Eroder.inl"
