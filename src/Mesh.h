/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class MeshAttributes;
class MeshConnectivity;
class MeshFactory;
class MeshGeometry;

/**
 * A barebone mesh structure for surface and volumetric meshes.
 *
 * This class is composed of 3 loosely coupled concepts:
 * 1. Geometry    : vertices, faces, voxels.
 * 2. Connectivity: who is connected to whom.
 * 3. Attributes  : values associated with each vertex/face/voxel.
 *
 * This design allows user to only deal with the Mesh object and remain ignorant
 * about the structure/logic underneath the facade.
 *
 * Limitation:
 * 1. only triangles are supported for now.
 */
class Mesh {
    public:
        // Use MeshFactory class to create mesh object.
        friend class MeshFactory;
        typedef std::shared_ptr<Mesh> Ptr;

    protected:
        Mesh();
        Mesh(Mesh& other) = delete;
        Mesh& operator=(Mesh& other) = delete;

    public:
        // Geometry access
        size_t get_dim() const;
        size_t get_num_vertices() const;
        size_t get_num_faces() const;
        size_t get_num_voxels() const;

        VectorF get_vertex(size_t i);
        VectorI get_face(size_t i);
        VectorI get_voxel(size_t i);

        VectorF get_vertex(size_t i) const;
        VectorI get_face(size_t i) const;
        VectorI get_voxel(size_t i) const;

        VectorF& get_vertices();
        VectorI& get_faces();
        VectorI& get_voxels();

        const VectorF& get_vertices() const;
        const VectorI& get_faces() const;
        const VectorI& get_voxels() const;

        int get_vertex_per_face() const;
        int get_vertex_per_voxel() const;

        // Connectivity access
        void enable_connectivity();
        void enable_vertex_connectivity();
        void enable_face_connectivity();
        void enable_voxel_connectivity();

        VectorI get_vertex_adjacent_vertices(size_t vi) const;
        VectorI get_vertex_adjacent_faces(size_t vi) const;
        VectorI get_vertex_adjacent_voxels(size_t vi) const;

        VectorI get_face_adjacent_faces(size_t fi) const;
        VectorI get_face_adjacent_voxels(size_t fi) const;

        VectorI get_voxel_adjacent_faces(size_t Vi) const;
        VectorI get_voxel_adjacent_voxels(size_t Vi) const;

        // Attribute access
        bool has_attribute(const std::string& attr_name) const;
        void add_attribute(const std::string& attr_name);
        void add_empty_attribute(const std::string& attr_name);
        void remove_attribute(const std::string& attr_name);
        VectorF& get_attribute(const std::string& attr_name);
        const VectorF& get_attribute(const std::string& attr_name) const;
        void set_attribute(const std::string& attr_name, VectorF& attr_value);
        std::vector<std::string> get_attribute_names() const;

    public:
        typedef std::shared_ptr<MeshGeometry>     GeometryPtr;
        typedef std::shared_ptr<MeshConnectivity> ConnectivityPtr;
        typedef std::shared_ptr<MeshAttributes>   AttributesPtr;

    protected:
        GeometryPtr     get_geometry() { return m_geometry; }
        ConnectivityPtr get_connectivity() { return m_connectivity; }
        AttributesPtr   get_attributes() { return m_attributes; }

        void set_geometry(GeometryPtr geometry);
        void set_connectivity(ConnectivityPtr connectivity);
        void set_attributes(AttributesPtr attributes);

    protected:
        GeometryPtr     m_geometry;
        ConnectivityPtr m_connectivity;
        AttributesPtr   m_attributes;
};

}
