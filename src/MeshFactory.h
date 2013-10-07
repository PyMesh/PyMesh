#pragma once
#include <string>
#include "EigenTypedef.h"
class Mesh;
class MeshParser;

/**
 * Typical usage:
 * Mesh* mesh = MeshFactory()
 *              .load_file("a.obj")
 *              .with_connectivity()
 *              .with_attribute("vertex_normal")
 *              .create();
 */
class MeshFactory {
    public:
        MeshFactory();
        MeshFactory& load_file(const std::string& filename);
        MeshFactory& load_data(VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t num_vertex_per_face, size_t num_vertex_per_voxel);
        MeshFactory& with_connectivity(std::string& conn_type);
        MeshFactory& with_attribute(const std::string& attr_name);
        Mesh* create() { return m_mesh; }

    private:
        // Disable evil copy constructors.
        // No implementation provided on purpose.
        MeshFactory(const MeshFactory& other);
        MeshFactory& operator=(const MeshFactory& other);

        void initialize_vertices(MeshParser* parser);
        void initialize_faces(MeshParser* parser);
        void initialize_voxels(MeshParser* parser);
        void initialize_attributes(MeshParser* parser);

    private:
        Mesh* m_mesh;
};
