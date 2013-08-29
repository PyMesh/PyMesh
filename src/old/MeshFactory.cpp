#include <iostream>

#include "MeshFactory.h"
#include "OBJMesh.h"

std::string get_extention(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    return filename.substr(pos);
}

Mesh* MeshFactory::create_mesh(const std::string& filename) {
    std::string ext = get_extention(filename);
    if (ext == ".obj") {
        Mesh* mesh = new OBJMesh();
        mesh->read(filename);
    } else {
        std::cerr << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        return NULL;
    }
}
