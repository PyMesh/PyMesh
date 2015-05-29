/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "OuterHullEngine.h"
#include <Core/Exception.h>
#ifdef WITH_IGL
#include "IGL/IGLOuterHullEngine.h"
#endif

#include <sstream>
#include <MeshUtils/IsolatedVertexRemoval.h>

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
    IsolatedVertexRemoval outer_hull_remover(m_vertices, m_faces);
    outer_hull_remover.run();
    m_vertices = outer_hull_remover.get_vertices();
    m_faces = outer_hull_remover.get_faces();

    IsolatedVertexRemoval interior_remover(m_interior_vertices, m_interior_faces);
    interior_remover.run();
    m_interior_vertices = interior_remover.get_vertices();
    m_interior_faces = interior_remover.get_faces();
}
