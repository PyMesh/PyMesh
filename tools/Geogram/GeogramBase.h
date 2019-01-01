/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_GEOGRAM

#include <memory>

#include <geogram/basic/common.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/basic/logger.h>
#include <geogram/mesh/mesh.h>


namespace PyMesh {

using GeoMesh = GEO::Mesh;
using GeoMeshPtr = std::shared_ptr<GeoMesh>;

/**
 * Base class for all GeoGram wrappers.
 * It takes care of initializing GeoGram environment.
 */
class GeogramBase {
    public:
        GeogramBase() {
            GEO::initialize();
            GEO::CmdLine::import_arg_group("standard");
            GEO::CmdLine::import_arg_group("pre");
            GEO::CmdLine::import_arg_group("algo");
            //GEO::Logger::instance()->set_quiet(true);
            //GEO::Logger::instance()->unregister_all_clients();
        }

        virtual ~GeogramBase() = default;
};

}

#endif
