/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Core/EigenTypedef.h>
#include <BVH/BVHEngine.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>

#include <memory>

namespace PyMesh {
namespace _CGAL {

class AABBTree : public BVHEngine {
    public:
        typedef std::shared_ptr<AABBTree> Ptr;

        typedef ::CGAL::Exact_predicates_inexact_constructions_kernel K;
        typedef K::Point_3 Point;
        typedef K::Segment_3 Segment;
        typedef K::Triangle_3 Triangle;

        typedef std::vector<Triangle> Triangles;
        typedef ::CGAL::AABB_triangle_primitive<K, Triangles::iterator> Primitive;
        typedef ::CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
        typedef ::CGAL::AABB_tree<AABB_triangle_traits> Tree;
        typedef std::shared_ptr<Tree> TreePtr;
        typedef Tree::Point_and_primitive_id Point_and_primitive_id;

    public:
        virtual ~AABBTree() = default;

        virtual void build();

        virtual void lookup(const MatrixFr& points,
                VectorF& squared_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points) const;

    private:
        TreePtr m_tree;
        Triangles m_triangles;
};


}
}

#endif
