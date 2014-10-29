#include "CSGEngine.h"

#ifdef WITH_CORK
#include "Cork/CorkEngine.h"
#endif

#include <sstream>
#include <iostream>

#include <Core/Exception.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>

namespace CSGEngineHelper {
    void engine_not_found(const std::string& engine_name) {
        std::stringstream err_msg;
        err_msg << "CSG engine \"" << engine_name
            << "\" is not supported." << std::endl;
        throw NotImplementedError(err_msg.str());
    }
}
using namespace CSGEngineHelper;

CSGEngine::Ptr CSGEngine::create(const std::string& engine_name) {
#ifdef WITH_CORK
    if (engine_name == "cork") { return Ptr(new CorkEngine()); }
#endif
    engine_not_found(engine_name);
    return Ptr(NULL);
}

void CSGEngine::clean_up() {
    remove_duplicated_vertices();
    remove_short_edges();
    remove_isolated_vertices();
}

void CSGEngine::remove_duplicated_vertices() {
    DuplicatedVertexRemoval remover(m_vertices, m_faces);
    size_t num_removed = remover.run(1e-6);
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void CSGEngine::remove_short_edges() {
    ShortEdgeRemoval remover(m_vertices, m_faces);
    size_t num_removed = remover.run(1e-6);
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void CSGEngine::remove_isolated_vertices() {
    IsolatedVertexRemoval remover(m_vertices, m_faces);
    size_t num_removed = remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}
