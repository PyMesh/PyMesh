#pragma once
#include <string>
#include <vector>
#include <map>

#include "EigenTypedef.h"
#include "MeshAttribute.h"

class Mesh;

class MeshAttributes {
    public:
        virtual ~MeshAttributes() {}

    public:
        typedef std::vector<std::string> AttributeNames;

    public:
        // Simple API
        virtual bool has_attribute(const std::string& name);
        virtual void add_empty_attribute(const std::string& name);
        virtual void add_attribute(const std::string& name, Mesh& mesh);
        virtual void remove_attribute(const std::string& name);
        virtual VectorF& get_attribute(const std::string& name);
        virtual void set_attribute(const std::string& name, VectorF& value);
        virtual AttributeNames get_attribute_names() const;

    protected:
        typedef std::map<std::string, MeshAttribute::Ptr> AttributeMap;
        typedef std::pair<std::string, MeshAttribute::Ptr> AttributeMapEntry;
        AttributeMap m_attributes;
};
