/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <map>
#include <string>
#include <vector>

#include "WireAttribute.h"

namespace PyMesh {

class WireNetwork;

class WireAttributes {
    public:
        bool has_attribute(const std::string& name) const;
        const MatrixFr& get_attribute(const std::string& name) const;
        void add_attribute(const WireNetwork& wire_network, const std::string& name,
                bool vertex_wise=true, bool auto_compute=true);
        void set_attribute(const WireNetwork& wire_network, const std::string& name, const MatrixFr& values);
        bool is_vertex_attribute(const std::string& name) const;
        void remove_attribute(const std::string& name);

        std::vector<std::string> get_attribute_names() const;

        void clear() { m_attr_map.clear(); }

    private:
        typedef std::map<std::string, WireAttribute::Ptr> AttrMap;
        AttrMap m_attr_map;
};

}
