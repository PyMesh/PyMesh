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

MeshParser::Ptr MeshParser::create_parser(const std::string& filename)
{
    std::string ext = IOUtils::get_extention(filename);
    return MeshParser::create_parser_for_extension(filename, ext);
}
MeshParser::Ptr MeshParser::create_parser_for_extension(const std::string& filename, const std::string& extension)
{
    Ptr parser;
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".obj") {
        parser = std::make_shared<OBJParser>();
    } else if (ext == ".off") {
        parser = std::make_shared<OFFParser>();
    } else if (ext == ".mesh") {
        parser = std::make_shared<MEDITParser>();
    } else if (ext == ".msh") {
        parser = std::make_shared<MSHParser>();
    } else if (ext == ".node") {
        parser = std::make_shared<NodeParser>();
    } else if (ext == ".stl") {
        parser = std::make_shared<STLParser>();
    } else if (ext == ".ply") {
        parser = std::make_shared<PLYParser>();
    } else if (ext == ".poly") {
        parser = std::make_shared<POLYParser>();
    } else if (ext == ".vega") {
        parser = std::make_shared<VEGAParser>();
    } else {
        std::stringstream err_msg;
        err_msg << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError(err_msg.str());
    }
    return parser;
}

