#include "MeshParser.h"

#include <iostream>
#include <sstream>

#include <Core/Exception.h>
#include "OBJParser.h"
#include "MSHParser.h"
#include "STLParser.h"
#include "PLYParser.h"
#include "IOUtils.h"

// Static factory method
MeshParser* MeshParser::create_parser(const std::string& filename) {
    MeshParser* parser = NULL;
    std::string ext = IOUtils::get_extention(filename);
    if (ext == ".obj") {
        parser= new OBJParser();
    } else if (ext == ".msh") {
        parser = new MSHParser();
    } else if (ext == ".stl") {
        parser = new STLParser();
    } else if (ext == ".ply") {
        parser = new PLYParser();
    } else {
        std::stringstream err_msg;
        err_msg << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError(err_msg.str());
    }
    return parser;
}

