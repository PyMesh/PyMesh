#pragma once
#include <string>
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
        MeshFactory& with_connectivity();
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
