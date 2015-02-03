#pragma once
#include <string>
#include <memory>

#include <Core/EigenTypedef.h>

class Mesh;

/**
 * Usage:
 *      MeshWriter::Ptr writer = MeshWriter::create("test.obj");
 *      writer->with_attribute("vertex_normal");
 *      writer->in_ascii();
 *      writer->write_mesh(mesh);
 * or
 *      MeshWriter::Ptr writer = MeshWriter::create("test.obj");
 *      writer->write(vertices, faces, voxels,
 *          dim, vertex_per_face, vertex_per_voxel);
 */
class MeshWriter {
    public:
        typedef std::shared_ptr<MeshWriter> Ptr;

        static MeshWriter* create_writer(const std::string& filename);
        static Ptr create(const std::string& filename);

    public:
        virtual ~MeshWriter() {}

    public:
        virtual void with_attribute(const std::string& attr_name)=0;
        virtual void in_ascii() {}

        virtual void write_mesh(Mesh& mesh)=0;
        virtual void write(const VectorF& vertices, const VectorI& faces, const VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel)=0;

    public:
        void set_output_filename(const std::string& filename) {
            m_filename = filename;
        }

    protected:
        std::string m_filename;
};
