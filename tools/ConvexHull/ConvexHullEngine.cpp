/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ConvexHullEngine.h"

#include <sstream>
#include <string>

#include <Core/Exception.h>

#ifdef WITH_CGAL
#include "CGAL/CGALConvexHull2D.h"
#include "CGAL/CGALConvexHull3D.h"
#endif

#ifdef WITH_QHULL
#include "Qhull/QhullEngine.h"
#endif

using namespace PyMesh;

namespace ConvexHullEngineHelper {
    bool is_positively_oriented(
            const Vector3F& v0, const Vector3F& v1, const Vector3F& v2) {
        return v0.cross(v1).dot(v2) > 0.0;
    }

    bool is_positively_oriented(
            const Vector2F& v0, const Vector2F& v1) {
        return v0[0]*v1[1] - v0[1]*v1[0] > 0.0;
    }
}

using namespace ConvexHullEngineHelper;

ConvexHullEngine::Ptr ConvexHullEngine::create(
        size_t dim, const std::string& library_name) {
#ifdef WITH_QHULL
    if (library_name == "qhull") {
        return Ptr(new QhullEngine);
    }
#endif

#ifdef WITH_CGAL
    if (library_name == "cgal") {
        if (dim == 2) {
            return Ptr(new CGALConvexHull2D);
        } else if (dim == 3) {
            return Ptr(new CGALConvexHull3D);
        } else {
            std::stringstream err_msg;
            err_msg << "CGAL convex hull does not support dim=" << dim;
            throw NotImplementedError(err_msg.str());
        }
    }
#endif

    std::stringstream err_msg;
    err_msg << "Convex hull library " << library_name
        << " is not supported.";
    throw NotImplementedError(err_msg.str());
}

bool ConvexHullEngine::supports(
        const std::string& library_name) {
#ifdef WITH_QHULL
    if ((library_name) == "qhull") return true;
#endif
#ifdef WITH_CGAL
    if ((library_name) == "cgal") return true;
#endif
    return false;
}

void ConvexHullEngine::reorient_faces() {
    const size_t dim = m_vertices.cols();
    const size_t num_faces = m_faces.rows();
    VectorF center = 0.5 * (
            m_vertices.colwise().minCoeff() +
            m_vertices.colwise().maxCoeff());
    if (dim == 2) {
        for (size_t i=0; i<num_faces; i++) {
            Vector2F v0 = m_vertices.row(m_faces(i,0)) - center.transpose();
            Vector2F v1 = m_vertices.row(m_faces(i,1)) - center.transpose();
            if (!is_positively_oriented(v0, v1)) {
                std::swap(m_faces.coeffRef(i,0), m_faces.coeffRef(i,1));
            }
        }
    } else if (dim == 3) {
        for (size_t i=0; i<num_faces; i++) {
            Vector3F v0 = m_vertices.row(m_faces(i,0)) - center.transpose();
            Vector3F v1 = m_vertices.row(m_faces(i,1)) - center.transpose();
            Vector3F v2 = m_vertices.row(m_faces(i,2)) - center.transpose();
            if (!is_positively_oriented(v0, v1, v2)) {
                std::swap(m_faces.coeffRef(i,0), m_faces.coeffRef(i,1));
            }
        }
    }
}

