#include "MeshParser.h"

#include <iostream>

#include "Exception.h"
#include "OBJParser.h"
#include "MSHParser.h"
#include "IOUtils.h"

using namespace Zhou;

// Static factory method
MeshParser* MeshParser::create_parser(const std::string& filename) {
    MeshParser* parser = NULL;
    std::string ext = get_extention(filename);
    if (ext == ".obj") {
        parser= new OBJParser();
    } else if (ext == ".msh") {
        parser = new MSHParser();
    } else {
        std::cerr << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError("Invalid file extention.");
    }
    return parser;
}

