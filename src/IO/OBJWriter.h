#pragma once

#include <Mesh.h>
#include "MeshWriter.h"

class OBJWriter : public MeshWriter {
    public:
        virtual ~OBJWriter() {}

    public:
        virtual void with_attribute(const std::string& attr_name);
        virtual void write_mesh(Mesh& mesh);
        virtual void write(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel);
};
