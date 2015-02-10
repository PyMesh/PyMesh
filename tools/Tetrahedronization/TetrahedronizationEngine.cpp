#include "TetrahedronizationEngine.h"

#include <sstream>

#include <Core/Exception.h>

#if WITH_CGAL
#include "CGAL/CGALMeshGen.h"
#endif

#if WITH_TETGEN
#include "TetGen/TetGenEngine.h"
#endif

TetrahedronizationEngine::Ptr TetrahedronizationEngine::create(
        const std::string& engine_name) {
#if WITH_CGAL
    if (engine_name == "cgal") { return Ptr(new CGALMeshGen); }
#endif
#if WITH_TETGEN
    if (engine_name == "tetgen") { return Ptr(new TetGenEngine); }
#endif

    std::stringstream err_msg;
    err_msg << "Tetrahedronization engine " << engine_name
        << " is not supported.";
    throw NotImplementedError(err_msg.str());
}
