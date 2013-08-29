#pragma once

#include "Mesh.h"
#include <iosfwd>

class OBJMesh : public Mesh {
    public:
        virtual ~OBJMesh() {}

        void read(const std::string& filename);
        void write(const std::string& filename);

    private:
        Vector3F parse_vertex_position(std::ifstream& fin);
        Vector3F parse_vertex_normal(std::ifstream& fin);
        Vector2F parse_vertex_texture(std::ifstream& fin);
};
