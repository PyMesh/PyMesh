/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "CompressionEngine.h"
#if WITH_DRACO
#include "Draco/DracoCompressionEngine.h"
#endif

using namespace PyMesh;

CompressionEngine::Ptr CompressionEngine::create(const std::string& engine_name) {
#if WITH_DRACO
    if (engine_name == "draco") {
        return std::make_shared<DracoCompressionEngine>();
    }
#endif
    throw NotImplementedError("Unsupported compression engine: " + engine_name);
}

