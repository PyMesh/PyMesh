/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Mesh.h>
#include <fstream>
#include "MeshWriter.h"

namespace PyMesh {

class MEDITWriter : public MeshWriter {
    public:
        virtual ~MEDITWriter() {}

    public:
        virtual void with_attribute(const std::string& attr_name);
        virtual void write_mesh(Mesh& mesh);
        virtual void write(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel);

    private:
        void write_vertices(
                std::ofstream& fout, const VectorF& vertices, const size_t dim);
        void write_faces(
                std::ofstream& fout, const VectorI& faces,
                const size_t vertex_per_face);
        void write_voxels(
                std::ofstream& fout, const VectorI& voxels,
                const size_t vertex_per_voxel);
};

}
