/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <Core/EigenTypedef.h>
#include "WireNetwork.h"

namespace PyMesh {

class WireWriter {
    public:
        WireWriter() {}

    public:
        void write(const std::string& filename, const WireNetwork& wires);
        void write_raw(const std::string& filename,
                const MatrixFr& vertices,
                const MatrixIr& edges);
};

}
