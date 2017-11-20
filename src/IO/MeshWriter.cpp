/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshWriter.h"

#include <iostream>

#include <Core/Exception.h>
#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "IOUtils.h"
#include "MEDITWriter.h"
#include "MSHWriter.h"
#include "NodeWriter.h"
#include "OBJWriter.h"
#include "OFFWriter.h"
#include "PLYWriter.h"
#include "POLYWriter.h"
#include "STLWriter.h"

using namespace PyMesh;

MeshWriter::Ptr MeshWriter::create(const std::string& filename) {
    Ptr writer;
    std::string ext = IOUtils::get_extention(filename);
    if (ext == ".obj") {
        writer = std::make_shared<OBJWriter>();
    } else if (ext == ".off") {
        writer = std::make_shared<OFFWriter>();
    } else if (ext == ".mesh") {
        writer = std::make_shared<MEDITWriter>();
    } else if (ext == ".msh") {
        writer = std::make_shared<MSHWriter>();
    } else if (ext == ".node") {
        writer = std::make_shared<NodeWriter>();
    } else if (ext == ".ply") {
        writer = std::make_shared<PLYWriter>();
    } else if (ext == ".poly") {
        writer = std::make_shared<POLYWriter>();
    } else if (ext == ".stl") {
        writer = std::make_shared<STLWriter>();
    } else {
        std::cerr << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError("Invalid file extention.");
    }

    writer->set_output_filename(filename);
    return writer;
}
