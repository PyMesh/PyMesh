#pragma once

#include <string>
#include <list>

#include "MeshWriter.h"

namespace Zhou {
class MshSaver;
}

class MSHWriter : public MeshWriter {
    public:
        virtual ~MSHWriter() {}

    public:
        virtual MeshWriter& with_attribute(const std::string& attr_name);
        virtual void write_mesh(Mesh& mesh);
        virtual void write(VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t vertex_per_face, size_t vertex_per_voxel);

    private:
        void write_volume_mesh(Mesh& mesh);
        void write_surface_mesh(Mesh& mesh);
        void write_geometry(Zhou::MshSaver& saver, VectorF& vertices,
                VectorI& elements, size_t vertex_per_element);
        void write_attribute(Zhou::MshSaver& saver, const std::string& name,
                VectorF& value, size_t num_vertices, size_t num_elements);

    private:
        typedef std::list<std::string> AttrNames;
        AttrNames m_attr_names;
};
