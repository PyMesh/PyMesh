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
        writer = Ptr(new OBJWriter());
    } else if (ext == ".off") {
        writer = Ptr(new OFFWriter());
    } else if (ext == ".mesh") {
        writer = Ptr(new MEDITWriter());
    } else if (ext == ".msh") {
        writer = Ptr(new MSHWriter());
    } else if (ext == ".node") {
        writer = Ptr(new NodeWriter());
    } else if (ext == ".ply") {
        writer = Ptr(new PLYWriter());
    } else if (ext == ".poly") {
        writer = Ptr(new POLYWriter());
    } else if (ext == ".stl") {
        writer = Ptr(new STLWriter());
    } else {
        std::cerr << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError("Invalid file extention.");
    }

    writer->set_output_filename(filename);
    return writer;
}
