/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Mesh.h>

#include <Core/Exception.h>

namespace PyMesh {

class CompressionEngine {
    public:
        using Ptr = std::shared_ptr<CompressionEngine>;
        static Ptr create(const std::string& engine_name);

    public:
        virtual ~CompressionEngine() = default;

    public:
        virtual std::string compress(Mesh::Ptr mesh) const {
            throw NotImplementedError("Compression algorithm is not implemented");
        }

        virtual Mesh::Ptr decompress(const std::string& data) const {
            throw NotImplementedError("Decompression algorithm is not implemented");
        }
};

}

