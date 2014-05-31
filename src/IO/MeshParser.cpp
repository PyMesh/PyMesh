#include "MeshParser.h"

#include <iostream>
#include <sstream>

#include <Core/Exception.h>
#include "OBJParser.h"
#include "MSHParser.h"
#include "STLParser.h"
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
    } else if (ext == ".stl") {
        parser = new STLParser();
    } else {
        std::stringstream err_msg;
        err_msg << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError(err_msg.str());
    }
    return parser;
}

