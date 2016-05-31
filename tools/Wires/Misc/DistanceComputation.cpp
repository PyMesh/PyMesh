/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "DistanceComputation.h"
#include <iostream>

using namespace PyMesh;
namespace DistanceComputationHelper {
    const Float epsilon = 1e-8;
}

using namespace DistanceComputation;
using namespace DistanceComputationHelper;

Float DistanceComputation::point_to_edge(
        const Vector3F& p,
        const Vector3F& e0,
        const Vector3F& e1) {
    Vector3F e = e1 - e0;
    Vector3F v =  p - e0;
    Float edge_len = e.norm();
    if (edge_len < epsilon) {
        return v.norm();
    }
    Float proj = v.dot(e);
    if (proj < 0.0)      return v.norm();
    if (proj > edge_len) return (p-e1).norm();
    return (v - e * (proj / (edge_len * edge_len))).norm();
}

Float DistanceComputation::point_to_triangle(
        const Vector3F& p,
        const Vector3F& c0,
        const Vector3F& c1,
        const Vector3F& c2) {
    Vector3F e1 = c1 - c0;
    Vector3F e2 = c2 - c0;
    Vector3F v  =  p - c0;
    Vector3F n = e1.cross(e2);
    Float area = n.norm();
    if (area < epsilon) {
        // triangle is tiny or degenerated.
        Float dist_to_edges[] = {
            point_to_edge(p, c0, c1),
            point_to_edge(p, c1, c2),
            point_to_edge(p, c2, c0)
        };
        return *std::min_element(dist_to_edges, dist_to_edges + 3);
    } else {
        // Normal can be trusted.
        n = n.normalized();
        Float dist_to_plane = v.dot(n);
        v = p - n * dist_to_plane;
        Float area_0 = (c1-c0).cross(v-c0).dot(n);
        if (area_0 < 0.0) return point_to_edge(p, c0, c1);
        Float area_1 = (c2-c1).cross(v-c1).dot(n);
        if (area_1 < 0.0) return point_to_edge(p, c1, c2);
        Float area_2 = (c0-c2).cross(v-c2).dot(n);
        if (area_2 < 0.0) return point_to_edge(p, c2, c0);
        return dist_to_plane;
    }
}
