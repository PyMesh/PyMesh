#pragma once
#include <string>
#include <memory>

#include <Core/EigenTypedef.h>

class Mesh;

/**
 * Usage:
 *      MeshWriter::create_writer("test.obj")
 *              .with_attribute("vertex_normal")
 *              .in_ascii()
 *              .write(mesh);
 * or
 *      MeshWriter::create_writer("test.obj")
 *              .write(vertices, faces, voxels);
 */
class MeshWriter {
    public:
        typedef std::shared_ptr<MeshWriter> Ptr;

        static MeshWriter* create_writer(const std::string& filename);
        static Ptr create(const std::string& filename);

    public:
        virtual ~MeshWriter() {}

    public:
        virtual MeshWriter& with_attribute(const std::string& attr_name)=0;
        virtual MeshWriter& in_ascii() { return *this; }

        virtual void write_mesh(Mesh& mesh)=0;
        virtual void write(VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel)=0;

    public:
        void set_output_filename(const std::string& filename) {
            m_filename = filename;
        }

    protected:
        std::string m_filename;
};
