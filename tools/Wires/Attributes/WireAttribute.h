/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cassert>
#include <string>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

class WireNetwork;

class WireAttribute {
    public:
        typedef std::shared_ptr<WireAttribute> Ptr;
        enum AttributeType { VERTEX, EDGE };

        static Ptr create(const std::string& name, AttributeType type);

    public:
        WireAttribute() : m_type(VERTEX) {}
        virtual ~WireAttribute() {}

        virtual void compute(const WireNetwork& network) { }

    public:
        size_t size() const { return m_values.rows(); }
        const MatrixFr& get_values() const { return m_values; }
        void set_values(const MatrixFr& value) { m_values = value; }
        AttributeType get_attribute_type() const { return m_type; }
        void set_attribute_type(AttributeType type) { m_type = type; }

    protected:
        AttributeType m_type;
        MatrixFr m_values;
};

}
