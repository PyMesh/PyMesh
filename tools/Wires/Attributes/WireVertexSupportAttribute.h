#pragma once

#include "WireAttribute.h"

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
