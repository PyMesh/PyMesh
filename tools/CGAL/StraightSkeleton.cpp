/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */

#include "StraightSkeleton.h"

#include <limits>
#include <list>
#include <vector>
#include <iostream>
#include <map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>

#include <Core/Exception.h>

using namespace PyMesh;

void StraightSkeleton::run(const MatrixFr& vertices, const MatrixIr& edges) {
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point_2 = Kernel::Point_2;
    using Polygon_2 = CGAL::Polygon_2<Kernel>;
    using PolygonWithHoles = CGAL::Polygon_with_holes_2<Kernel>;

    if (vertices.cols() != 2) {
        throw NotImplementedError("Only 2D straight skeleton is supported.");
    }
    if (edges.cols() != 2) {
        throw RuntimeError("Unexpected edge matrix with "
                + std::to_string(edges.cols()) + " columns.");
    }

    const size_t num_vertices = vertices.rows();
    const size_t num_edges = edges.rows();
    constexpr size_t INVALID = std::numeric_limits<size_t>::max();
    std::vector<size_t> next(num_vertices, INVALID);
    for (size_t i=0; i<num_edges; i++) {
        next[edges(i,0)] = edges(i, 1);
    }

    std::list<Polygon_2> outer_polygons;
    std::list<Polygon_2> holes;
    std::vector<bool> visited(num_vertices, false);
    for (size_t i=0; i<num_vertices; i++) {
        if (visited[i]) continue;

        Polygon_2 polygon;
        size_t idx = i;
        while(idx != INVALID && !visited[idx]) {
            visited[idx] = true;
            polygon.push_back(Point_2(vertices(idx,0), vertices(idx, 1)));
            idx = next[idx];
        }
        const auto orientation = polygon.orientation();
        switch (orientation) {
            case CGAL::POSITIVE:
                outer_polygons.push_back(std::move(polygon));
                break;
            case CGAL::NEGATIVE:
                holes.push_back(std::move(polygon));
                break;
            default:
                std::cerr << "Skipping exactly degenerated polygons"
                    << std::endl;
                break;
        }
    }

    if (outer_polygons.size() != 1) {
        throw NotImplementedError("Multiple outer polygons are not supported");
    }

    const auto& outer_polygon = outer_polygons.front();

    PolygonWithHoles domain(outer_polygon);
    for (const auto& hole : holes) {
        domain.add_hole(hole);
    }

    const auto skeleton = CGAL::create_interior_straight_skeleton_2(domain);
    std::map<decltype(skeleton)::element_type::Vertex_handle, size_t> index_map;

    const size_t num_nodes = skeleton->size_of_vertices();
    m_skeleton_vertices.resize(num_nodes, 2);
    size_t count = 0;
    for (auto itr = skeleton->vertices_begin(); itr != skeleton->vertices_end(); itr++) {
        const auto& p = itr->point();
        m_skeleton_vertices.row(count) << p[0], p[1];
        index_map[itr] = count;
        count++;
    }

    std::vector<size_t> skeleton_edges;
    skeleton_edges.reserve(num_nodes * 2);
    size_t num_segments = 0;
    for (auto itr=skeleton->halfedges_begin(); itr != skeleton->halfedges_end(); itr++) {
        if (itr->is_border() || itr->opposite()->is_border()) continue;
        const size_t vid0 = index_map[itr->vertex()];
        const size_t vid1 = index_map[itr->opposite()->vertex()];

        if (vid0 < vid1) {
            num_segments += 1;
            skeleton_edges.push_back(vid0);
            skeleton_edges.push_back(vid1);
        }
    }

    m_skeleton_edges.resize(num_segments, 2);
    std::copy(skeleton_edges.begin(), skeleton_edges.end(),
            m_skeleton_edges.data());
}

