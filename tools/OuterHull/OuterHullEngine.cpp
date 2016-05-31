/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OuterHullEngine.h"
#include <Core/Exception.h>
#ifdef WITH_IGL
#include "IGL/IGLOuterHullEngine.h"
#endif

#include <sstream>
#include <MeshUtils/IsolatedVertexRemoval.h>

using namespace PyMesh;

OuterHullEngine::Ptr OuterHullEngine::create(const std::string& engine_name) {
#ifdef WITH_IGL
    if (engine_name == "igl") {
        return std::make_shared<IGLOuterHullEngine>();
    }
#endif

    std::stringstream err_msg;
    err_msg << "Unknown outer hull engine: " << engine_name;
    throw NotImplementedError(err_msg.str());
}

void OuterHullEngine::remove_isolated_vertices() {
    IsolatedVertexRemoval remover(m_vertices, m_faces);
    remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}
