/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#ifdef WITH_GEOGRAM

#include <geogram/basic/common.h>

namespace PyMesh {

/**
 * Base class for all GeoGram wrappers.
 * It takes care of initializing GeoGram environment.
 */
class GeoGramBase {
    public:
        GeoGramBase() {
            if (!m_initialized) {
                GEO::initialize();
                m_initialized = true;
            }
        }

        virtual ~GeoGramBase() = default;
    private:
        static bool m_initialized;
};

}

#endif
