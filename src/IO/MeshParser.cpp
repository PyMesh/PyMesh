/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshParser.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <Core/Exception.h>
#include "OBJParser.h"
#include "OFFParser.h"
#include "MEDITParser.h"
#include "MSHParser.h"
#include "NodeParser.h"
#include "STLParser.h"
#include "PLYParser.h"
#include "POLYParser.h"
#include "IOUtils.h"

using namespace PyMesh;

// Static factory method
MeshParser* MeshParser::create_parser(const std::string& filename) {
    MeshParser* parser = NULL;
    std::string ext = IOUtils::get_extention(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".obj") {
        parser= new OBJParser();
    } else if (ext == ".off") {
        parser= new OFFParser();
    } else if (ext == ".mesh") {
        parser = new MEDITParser();
    } else if (ext == ".msh") {
        parser = new MSHParser();
    } else if (ext == ".node") {
        parser = new NodeParser();
    } else if (ext == ".stl") {
        parser = new STLParser();
    } else if (ext == ".ply") {
        parser = new PLYParser();
    } else if (ext == ".poly") {
        parser = new POLYParser();
    } else {
        std::stringstream err_msg;
        err_msg << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError(err_msg.str());
    }
    return parser;
}

