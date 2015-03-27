/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <set>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>

#include "CGAL_includes.h"

class SelfIntersection {
    public:
        SelfIntersection(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        void detect_self_intersection();

        void clear();

        MatrixIr get_self_intersecting_pairs() const {
            if (!m_intersecting_pairs.empty())
                return MatrixUtils::rowstack(m_intersecting_pairs);
            else
                return MatrixIr::Zero(0, 2);
        }

    public:
        void handle_intersection_candidate(
                const TrianglesIterator& b1, const TrianglesIterator& b2);

    private:
        std::vector<Box> get_triangle_bboxes();
        size_t topological_overlap(size_t id1, size_t id2) const;

    private:
        std::vector<Vector2I> m_intersecting_pairs;
        Triangles m_mesh;
        MatrixFr m_vertices;
        MatrixIr m_faces;
};
