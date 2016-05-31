/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>

#include <Core/EigenTypedef.h>

namespace PyMesh {
class Mesh;

/**
 * MeshAttribute provides functionality to compute and store information
 * associated with each of the mesh internal structure.  For example, vertex
 * normal, face area, voxel volumes, etc.
 */
class MeshAttribute {
    public:
        typedef std::shared_ptr<MeshAttribute> Ptr;

    public:
        MeshAttribute(const std::string& name) : m_name(name) {}
        virtual ~MeshAttribute() {}

    public:
        virtual void compute_from_mesh(Mesh& mesh) {}
        virtual const std::string& get_name() const { return m_name; }
        virtual VectorF& get_values() { return m_values; }
        virtual void set_values(VectorF& values) { m_values = values; }

    protected:
        std::string m_name;
        VectorF m_values;
};
}
