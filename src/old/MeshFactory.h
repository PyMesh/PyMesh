#pragma once
#include <string>
#include "Mesh.h"

class MeshFactory {
    public:
        static Mesh* create_mesh(const std::string& filename);
};
