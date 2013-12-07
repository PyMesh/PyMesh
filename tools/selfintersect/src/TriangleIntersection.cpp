#include "TriangleIntersection.h"
#include <cassert>
#include <vector>
#include <cmath>
#include <iostream>

#include <Core/Exception.h>

#include "CGAL_includes.hpp"

namespace TriangleIntersectionHelper {
    std::vector<int > get_shared_vertices(
            const Triangle_3& tri1,
            const Triangle_3& tri2) {
        std::vector<int> shared_vertices;
        for (size_t i=0; i<3; i++) {
            for (size_t j=0; j<3; j++) {
                if (tri1[i] == tri2[j]) {
                    shared_vertices.push_back(i);
                    shared_vertices.push_back(j);
                    break;
                }
            }
        }
        return shared_vertices;
    }

    bool do_intersect_single_shared_vertex(
            const Triangle_3& tri1,
            const Triangle_3& tri2,
            const std::vector<int>& shared_vertices) {
        assert(shared_vertices.size() == 2);
        int t1_v0 = shared_vertices[0];
        int t1_v1 = (t1_v0+1) % 3;
        int t1_v2 = (t1_v0+2) % 3;

        int t2_v0 = shared_vertices[1];
        int t2_v1 = (t2_v0+1) % 3;
        int t2_v2 = (t2_v0+2) % 3;
        Segment_3 edge_1(tri1[t1_v1], tri1[t1_v2]);
        Segment_3 edge_2(tri2[t2_v1], tri2[t2_v2]);
        return CGAL::do_intersect(edge_1, tri2) ||
            CGAL::do_intersect(edge_2, tri1);
    }

    bool do_intersect_double_shared_vertex(
            const Triangle_3& tri1,
            const Triangle_3& tri2,
            const std::vector<int>& shared_vertices) {
        assert(shared_vertices.size() == 4);
        int t1_v0 = shared_vertices[0];
        int t1_v1 = shared_vertices[2];
        int t1_v2 = (t1_v0+1)%3 == t1_v1 ? (t1_v1+1)%3 : (t1_v0+1)%3;
        assert(t1_v0 != t1_v1);

        int t2_v0 = shared_vertices[1];
        int t2_v1 = shared_vertices[3];
        int t2_v2 = (t2_v0+1)%3 == t2_v1 ? (t2_v1+1)%3 : (t2_v0+1)%3;
        assert(t2_v0 != t2_v1);

        const Point_3& p0 = tri1[t1_v0];
        const Point_3& p1 = tri1[t1_v1];
        const Point_3& p2 = tri1[t1_v2];
        const Point_3& p3 = tri2[t2_v2];

        // Two non-coplanar triangles with an shared edge will never intersect.
        if (!CGAL::coplanar(p0, p1, p2, p3)) { return false; }

        Vector_3 shared_edge = p1 - p0;
        Vector_3 edge_1 = p2 - p0;
        Vector_3 edge_2 = p3 - p0;
        Kernel::FT orientation = CGAL::cross_product(edge_1, shared_edge) *
            CGAL::cross_product(edge_2, shared_edge);

        // If the triangles are coplanar and have the same orientation, they
        // must intersect.  In particular, if one or both triangles are
        // degenerated, orientation would be exactly 0.  We need to handle this
        // case as well.
        return orientation >= 0;
    }

    CGAL::Object intersect_single_shared_vertex(
            const Triangle_3& tri1,
            const Triangle_3& tri2,
            const std::vector<int>& shared_vertices) {
        assert(shared_vertices.size() == 2);
        int t1_v0 = shared_vertices[0];
        const Point_3& common_vertex = tri1[t1_v0];

        CGAL::Object result = CGAL::intersection(tri1, tri2);
        if (result.is_empty()) {
            throw RuntimeError("Assumption wrong!  Triangles were assumed to share 1 vertex.");
        } else if (const Point_3* p = CGAL::object_cast<Point_3>(&result)) {
            if (*p != common_vertex) {
                throw RuntimeError("Assumption wrong!  Triangles does not intersect at the shared vertex.");
            }
            // Ignore the common shared vertex
            CGAL::Object empty;
            result = empty;
        }
        return result;
    }

    CGAL::Object intersect_double_shared_vertex(
            const Triangle_3& tri1,
            const Triangle_3& tri2,
            const std::vector<int>& shared_vertices) {
        assert(shared_vertices.size() == 4);
        int t1_v0 = shared_vertices[0];
        int t1_v1 = shared_vertices[2];
        int t1_v2 = (t1_v0+1)%3 == t1_v1 ? (t1_v1+1)%3 : (t1_v0+1)%3;

        int t2_v0 = shared_vertices[1];
        int t2_v1 = shared_vertices[3];
        int t2_v2 = (t2_v0+1)%3 == t2_v1 ? (t2_v1+1)%3 : (t2_v0+1)%3;

        const Point_3& p0 = tri1[t1_v0];
        const Point_3& p1 = tri1[t1_v1];
        const Point_3& p2 = tri1[t1_v2];
        const Point_3& p3 = tri2[t2_v2];

        CGAL::Object result = CGAL::intersection(tri1, tri2);
        if (result.is_empty()) {
            throw RuntimeError("Assumption wrong!  Triangles were assumed to share 2 vertex.");
        } else if (const Point_3* p = CGAL::object_cast<Point_3>(&result)) {
            throw RuntimeError("Assumption wrong!  Triangles were assumed to share 2 vertex.");
        } else if (const Segment_3* s = CGAL::object_cast<Segment_3>(&result)) {
            if ((s->source() == p0 && s->target() == p1) ||
                    (s->source() == p1 && s->target() == p0)) {
                CGAL::Object empty;
                result = empty;
            } else {
                throw RuntimeError("Assumption wrong!  Triangles does not intersect at the shared edge.");
            }
        }

        return result;
    }

}

using namespace TriangleIntersection;
using namespace TriangleIntersectionHelper;

bool TriangleIntersection::detect(
        const Triangle_3& tri1,
        const Triangle_3& tri2,
        bool ignore_shared_vertices) {
    if (ignore_shared_vertices) {
        return CGAL::do_intersect(tri1, tri2);
    }
    std::vector<int> shared_vertices = get_shared_vertices(tri1, tri2);
    if (shared_vertices.size()/2 == 0) {
        return CGAL::do_intersect(tri1, tri2);
    } else if (shared_vertices.size()/2 == 1) {
        return do_intersect_single_shared_vertex(tri1, tri2, shared_vertices);
    } else if (shared_vertices.size()/2 == 2) {
        return do_intersect_double_shared_vertex(tri1, tri2, shared_vertices);
    } else {
        std::cerr << "Triangles are geometrically identical." << std::endl;
        return false;
    }
}

CGAL::Object TriangleIntersection::resolve(
        const Triangle_3& tri1,
        const Triangle_3& tri2,
        bool ignore_shared_vertices) {
    CGAL::Object intersection;
    if (!detect(tri1, tri2, ignore_shared_vertices)) {
        return intersection;
    }

    if (ignore_shared_vertices) {
        intersection = CGAL::intersection(tri1, tri2);
    } else {
        std::vector<int> shared_vertices = get_shared_vertices(tri1, tri2);
        if (shared_vertices.size()/2 == 0) {
            intersection = CGAL::intersection(tri1, tri2);
        } else if (shared_vertices.size()/2 == 1) {
            intersection = intersect_single_shared_vertex(tri1, tri2, shared_vertices);
        } else if (shared_vertices.size()/2 == 2) {
            intersection = intersect_double_shared_vertex(tri1, tri2, shared_vertices);
        } else {
            std::cerr << "Triangles are geometrically identical." << std::endl;
        }
    }
    return intersection;
}



