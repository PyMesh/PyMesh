/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "BooleanEngine.h"

#ifdef WITH_CORK
#include "Cork/CorkEngine.h"
#endif
#ifdef WITH_CLIPPER
#include "Clipper/ClipperEngine.h"
#endif
#ifdef WITH_IGL
#include "IGL/IGLEngine.h"
#endif 
#ifdef WITH_CGAL
#include "CGAL/CGALBooleanEngine.h"
#include "CGAL/CGALCorefinementEngine.h"
#endif 
#ifdef WITH_CARVE
#include "Carve/CarveEngine.h"
#endif 
#ifdef WITH_BSP
#include "BSP/BSPEngine.h"
#endif

#include <sstream>
#include <iostream>

#include <Core/Exception.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>

using namespace PyMesh;

BooleanEngine::Ptr BooleanEngine::create(const std::string& engine_name) {
#ifdef WITH_CORK
    if (engine_name == "cork") { return Ptr(new CorkEngine()); }
#endif
#ifdef WITH_CLIPPER
    if (engine_name == "clipper") { return Ptr(new ClipperEngine()); }
#endif
#ifdef WITH_IGL
    if (engine_name == "igl") { return Ptr(new IGLEngine()); }
#endif
#ifdef WITH_CGAL
    if (engine_name == "cgal") { return Ptr(new CGALBooleanEngine()); }
    if (engine_name == "corefinement") {
        return Ptr(new CGALCorefinementEngine());
    }
#endif
#ifdef WITH_CARVE
    if (engine_name == "carve") { return Ptr(new CarveEngine()); }
#endif
#ifdef WITH_BSP
    if (engine_name == "bsp") { return Ptr(new BSPEngine()); }
#endif
    std::stringstream err_msg;
    err_msg << "Boolean engine \"" << engine_name
        << "\" is not supported." << std::endl;
    throw NotImplementedError(err_msg.str());
}

void BooleanEngine::clean_up() {
    remove_duplicated_vertices();
    remove_short_edges();
    remove_isolated_vertices();
}

void BooleanEngine::remove_duplicated_vertices() {
    DuplicatedVertexRemoval remover(m_vertices, m_faces);
    size_t num_removed = remover.run(1e-6);
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void BooleanEngine::remove_short_edges() {
    ShortEdgeRemoval remover(m_vertices, m_faces);
    size_t num_removed = remover.run(1e-6);
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void BooleanEngine::remove_isolated_vertices() {
    IsolatedVertexRemoval remover(m_vertices, m_faces);
    size_t num_removed = remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void BooleanEngine::serialize_xml(const std::string& filename) const {
    throw NotImplementedError("Serialization is not supported");
}
