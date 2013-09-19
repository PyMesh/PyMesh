#pragma once

#include "CGAL_includes.hpp"

namespace TriangleIntersection {
    bool detect(
            const Triangle_3& tri1,
            const Triangle_3& tri2,
            bool ignore_shared_vertices=true);

    CGAL::Object resolve(
            const Triangle_3& tri1,
            const Triangle_3& tri2,
            bool ignore_shared_vertices=true);
}
