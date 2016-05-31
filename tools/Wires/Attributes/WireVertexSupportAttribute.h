/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "WireAttribute.h"

namespace PyMesh {

class WireVertexSupportAttribute : public WireAttribute {
    public:
        WireVertexSupportAttribute(const Vector3F& print_dir)
            : m_print_dir(print_dir) {
                set_attribute_type(VERTEX);
            }
        virtual ~WireVertexSupportAttribute() {}

    public:
        virtual void compute(const WireNetwork& network);

    protected:
        void mark_directly_supported_vertices(
                const WireNetwork& network, MatrixFr& markers);
        void propagate_supports(
                const WireNetwork& network, MatrixFr& markers);

    protected:
        Vector3F m_print_dir;
};

}
