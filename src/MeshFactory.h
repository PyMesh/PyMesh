#pragma once
#include <string>
#include <Core/EigenTypedef.h>

#include "Mesh.h"

namespace PyMesh {
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
        MeshFactory& load_data(
                const VectorF& vertices, const VectorI& faces, const VectorI& voxels,
                size_t dim, size_t num_vertex_per_face, size_t num_vertex_per_voxel);
        MeshFactory& with_connectivity(const std::string& conn_type);
        MeshFactory& with_attribute(const std::string& attr_name);
        MeshFactory& drop_zero_dim();
        Mesh* create() { return m_mesh; }
        Mesh::Ptr create_shared() { return Mesh::Ptr(m_mesh); }

    private:
        // Disable evil copy constructors.
        // No implementation provided on purpose.
        MeshFactory(const MeshFactory& other);
        MeshFactory& operator=(const MeshFactory& other);

        void initialize_vertices(MeshParser* parser);
        void initialize_faces(MeshParser* parser);
        void initialize_voxels(MeshParser* parser);
        void initialize_attributes(MeshParser* parser);
        void compute_and_drop_zero_dim();

    private:
        Mesh* m_mesh;
};
}
