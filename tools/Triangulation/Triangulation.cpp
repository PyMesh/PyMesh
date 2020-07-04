/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "Triangulation.h"

#ifdef WITH_IGL_AND_CGAL
#include "IGL/LexicographicTriangulation.h"
#include "IGL/DelaunayTriangulation.h"
#endif

#ifdef WITH_CGAL
#include "CGAL/ConstrainedDelaunay.h"
#include "CGAL/ConformingDelaunay.h"
#endif

#ifdef WITH_TRIANGLE
#include "ShewchukTriangle/ConformingDelaunay.h"
#include "ShewchukTriangle/ConstrainedDelaunay.h"
#include "ShewchukTriangle/Refiner.h"
#endif

#ifdef WITH_GEOGRAM
#include "Geogram/GeogramDelaunayTriangulation.h"
#endif

#ifdef WITH_JIGSAW
#include "Jigsaw/FrontalDelaunay.h"
#endif

#ifdef WITH_MMG
#include "MMG/Delaunay.h"
#endif

#include <Core/Exception.h>
#include <sstream>

using namespace PyMesh;

Triangulation::Ptr Triangulation::create(const std::string& engine_name) {
#ifdef WITH_IGL_AND_CGAL
    if (engine_name == "igl_lexicographic") {
        return Ptr(new LexicographicTriangulation());
    }
#endif

#ifdef WITH_IGL_AND_CGAL
    if (engine_name == "igl_delaunay") {
        return Ptr(new DelaunayTriangulation());
    }
#endif

#ifdef WITH_TRIANGLE
    if (engine_name == "triangle_conforming_delaunay") {
        return Ptr(new Triangle::ConformingDelaunay());
    } else if (engine_name == "triangle_constrained_delaunay") {
        return Ptr(new Triangle::ConstrainedDelaunay());
    } else if (engine_name == "triangle_refiner") {
        return Ptr(new Triangle::Refiner());
    }
#endif

#ifdef WITH_CGAL
    if (engine_name == "cgal_constrained_delaunay") {
        return Ptr(new PyMesh::_CGAL::ConstrainedDelaunay());
    } else if (engine_name == "cgal_conforming_delaunay") {
        return Ptr(new PyMesh::_CGAL::ConformingDelaunay());
    }
#endif

#ifdef WITH_GEOGRAM
    if (engine_name == "geogram_delaunay") {
        return Ptr(new GeogramDelaunayTriangulation());
    }
#endif

#ifdef WITH_JIGSAW
    if (engine_name == "jigsaw_frontal_delaunay") {
        return Ptr(new Jigsaw::FrontalDelaunay());
    }
#endif

#ifdef WITH_MMG
    if (engine_name == "mmg_delaunay") {
        return Ptr(new MMG::Delaunay());
    }
#endif

    std::stringstream err_msg;
    err_msg << "Triangulation engine " << engine_name
        << " is not supported";
    throw NotImplementedError(err_msg.str());
}

bool Triangulation::supports(const std::string& engine_name) {
#ifdef WITH_IGL_AND_CGAL
    if (engine_name == "igl_lexicographic") return true;
    if (engine_name == "igl_delaunay") return true;
#endif
#ifdef WITH_TRIANGLE
    if (engine_name == "triangle_conforming_delaunay") return true;
    if (engine_name == "triangle_constrained_delaunay") return true;
    if (engine_name == "triangle_refiner") return true;
#endif
#ifdef WITH_CGAL
    if (engine_name == "cgal_constrained_delaunay") return true;
    if (engine_name == "cgal_conforming_delaunay") return true;
#endif
#ifdef WITH_GEOGRAM
    if (engine_name == "geogram_delaunay") return true;
#endif
#ifdef WITH_JIGSAW
    if (engine_name == "jigsaw_frontal_delaunay") return true;
#endif
#ifdef WITH_MMG
    if (engine_name == "mmg_delaunay") return true;
#endif
    return false;
}

std::vector<std::string> Triangulation::get_available_engines() {
    std::vector<std::string> engine_names;
#ifdef WITH_IGL_AND_CGAL
    engine_names.push_back("igl_lexicographic");
    engine_names.push_back("igl_delaunay");
#endif
#ifdef WITH_TRIANGLE
    engine_names.push_back("triangle_conforming_delaunay");
    engine_names.push_back("triangle_constrained_delaunay");
    engine_names.push_back("triangle_refiner");
#endif
#ifdef WITH_CGAL
    engine_names.push_back("cgal_constrained_delaunay");
    engine_names.push_back("cgal_conforming_delaunay");
#endif
#ifdef WITH_GEOGRAM
    engine_names.push_back("geogram_delaunay");
#endif
#ifdef WITH_JIGSAW
    engine_names.push_back("jigsaw_frontal_delaunay");
#endif
#ifdef WITH_MMG
    engine_names.push_back("mmg_delaunay");
#endif
    return engine_names;
}
