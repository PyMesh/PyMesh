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
#include "VEGAParser.h"
#include "IOUtils.h"

using namespace PyMesh;

// Static factory method
MeshParser::Ptr MeshParser::create_parser(const std::string& filename) {
    Ptr parser;
    std::string ext = IOUtils::get_extention(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".obj") {
        parser = Ptr(new OBJParser());
    } else if (ext == ".off") {
        parser = Ptr(new OFFParser());
    } else if (ext == ".mesh") {
        parser = Ptr(new MEDITParser());
    } else if (ext == ".msh") {
        parser = Ptr(new MSHParser());
    } else if (ext == ".node") {
        parser = Ptr(new NodeParser());
    } else if (ext == ".stl") {
        parser = Ptr(new STLParser());
    } else if (ext == ".ply") {
        parser = Ptr(new PLYParser());
    } else if (ext == ".poly") {
        parser = Ptr(new POLYParser());
    } else if (ext == ".vega") {
        parser = Ptr(new VEGAParser());
    } else {
        std::stringstream err_msg;
        err_msg << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError(err_msg.str());
    }
    return parser;
}

