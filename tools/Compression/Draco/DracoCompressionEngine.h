/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_DRACO

#include <Compression/CompressionEngine.h>

#include <string>

namespace PyMesh {

class DracoCompressionEngine : public CompressionEngine {
    public:
        virtual std::string compress(Mesh::Ptr mesh) const;
        virtual Mesh::Ptr decompress(const std::string& data) const;
};

}

#endif
