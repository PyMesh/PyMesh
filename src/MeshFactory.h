#pragma once
#include <string>
#include <Core/EigenTypedef.h>

#include <Mesh.h>
#include <IO/MeshParser.h>

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
        MeshFactory& load_file_with_hint(const std::string& filename, const std::string& extension_hint);
        MeshFactory& load_data(
                const VectorF& vertices, const VectorI& faces, const VectorI& voxels,
                size_t dim, size_t num_vertex_per_face, size_t num_vertex_per_voxel);
        MeshFactory& load_matrices(
                const MatrixFr& vertices, const MatrixIr& faces, const MatrixIr& voxels);
        MeshFactory& with_connectivity(const std::string& conn_type);
        MeshFactory& with_attribute(const std::string& attr_name);
        MeshFactory& drop_zero_dim();
        Mesh::Ptr create() { return m_mesh; }

    private:
        MeshFactory(const MeshFactory& other) = delete;
        MeshFactory& operator=(const MeshFactory& other) = delete;

        void initialize_vertices(MeshParser::Ptr parser);
        void initialize_faces(MeshParser::Ptr parser);
        void initialize_voxels(MeshParser::Ptr parser);
        void initialize_attributes(MeshParser::Ptr parser);
        void compute_and_drop_zero_dim();

    private:
        std::shared_ptr<Mesh> m_mesh;
};
}
