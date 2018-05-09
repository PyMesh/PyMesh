/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#ifdef WITH_GEOGRAM

#include <geogram/basic/common.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/basic/logger.h>

namespace PyMesh {

/**
 * Base class for all GeoGram wrappers.
 * It takes care of initializing GeoGram environment.
 */
class GeogramBase {
    public:
        GeogramBase() {
            if (!m_initialized) {
                GEO::initialize();
                GEO::Logger::instance()->set_quiet(true);
                GEO::Logger::instance()->unregister_all_clients();
                m_initialized = true;
            }
        }

        virtual ~GeogramBase() = default;
    private:
        static bool m_initialized;
};

}

#endif
