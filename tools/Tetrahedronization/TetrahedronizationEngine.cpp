#include "TetrahedronizationEngine.h"

#include <sstream>

#include <Core/Exception.h>

#include "CGAL/CGALMeshGen.h"

TetrahedronizationEngine::Ptr TetrahedronizationEngine::create(
        const std::string& engine_name) {
#if WITH_CGAL
    if (engine_name == "cgal") { return Ptr(new CGALMeshGen); }
#endif

    std::stringstream err_msg;
    err_msg << "Tetrahedronization engine " << engine_name
        << " is not supported.";
    throw NotImplementedError(err_msg.str());
}
