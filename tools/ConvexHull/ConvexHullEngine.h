/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <memory>

#include <Core/EigenTypedef.h>

namespace PyMesh {

/**
 * ConvexHullEngine defines the minimalistic interface that compute the convex
 * hull of a set of input points.  The output is the vertices and faces of the
 * triangulated convex hull.  Each face is correctly orienated with normal
 * pointing away from the center.  In addition, the algorithm also returns an
 * index for each convex hull vertex that specifies the corresponding input
 * point.
 */
class ConvexHullEngine {
    public:
        typedef std::shared_ptr<ConvexHullEngine> Ptr;
        static Ptr create(size_t dim, const std::string& library_name);
        static bool supports(const std::string& library_name);

    public:
        virtual ~ConvexHullEngine() {}

        virtual void run(const MatrixFr& points)=0;

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_index_map() const { return m_index_map; }

    protected:
        void reorient_faces();

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_index_map;
};

}
