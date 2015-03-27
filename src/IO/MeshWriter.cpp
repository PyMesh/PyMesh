/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshWriter.h"

#include <iostream>

#include <Core/Exception.h>
#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "IOUtils.h"
#include "MSHWriter.h"
#include "OBJWriter.h"
#include "PLYWriter.h"

MeshWriter* MeshWriter::create_writer(const std::string& filename) {
    MeshWriter* writer = NULL;
    std::string ext = IOUtils::get_extention(filename);
    if (ext == ".obj") {
        writer = new OBJWriter();
    } else if (ext == ".msh") {
        writer = new MSHWriter();
    } else if (ext == ".ply") {
        writer = new PLYWriter();
    } else {
        std::cerr << "Error: extention (" << ext << ") is not supported"
            << std::endl;
        throw IOError("Invalid file extention.");
    }

    writer->set_output_filename(filename);
    return writer;
}

MeshWriter::Ptr MeshWriter::create(const std::string& filename) {
    return Ptr(create_writer(filename));
}
