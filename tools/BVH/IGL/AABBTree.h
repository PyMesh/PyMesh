/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <Core/EigenTypedef.h>
#include <BVH/BVHEngine.h>

#include <igl/AABB.h>
#include <igl/signed_distance.h>

namespace PyMesh {
namespace IGL {

template<int DIM>
class AABBTree : public BVHEngine {};

template<>
class AABBTree<2> : public BVHEngine {
    public:
        using Ptr = std::shared_ptr<AABBTree>;
        using Tree = igl::AABB<MatrixFr, 2>;

    public:
        virtual ~AABBTree() = default;

        virtual void build() override {
            m_tree.init(m_vertices, m_faces);
        }

        virtual void lookup(const MatrixFr& points,
                VectorF& squared_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points) const override {
            m_tree.squared_distance(m_vertices, m_faces, points,
                    squared_distances, closest_faces, closest_points);
        }

        virtual void lookup_signed(const MatrixFr& points,
                const MatrixFr& face_normals,
                const MatrixFr& vertex_normals,
                const MatrixFr& edge_normals,
                const VectorI& edge_map,
                VectorF& signed_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points,
                MatrixFr& closest_face_normals) const override {
            throw NotImplementedError("Signed distance in 2D is not yet supported.");
        }

    private:
        Tree m_tree;
};

template<>
class AABBTree<3> : public BVHEngine {
    public:
        using Ptr = std::shared_ptr<AABBTree>;
        using Tree = igl::AABB<MatrixFr, 3>;

    public:
        virtual ~AABBTree() = default;

        virtual void build() override {
            m_tree.init(m_vertices, m_faces);
        }

        virtual void lookup(const MatrixFr& points,
                VectorF& squared_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points) const override {
            m_tree.squared_distance(m_vertices, m_faces, points,
                    squared_distances, closest_faces, closest_points);
        }

        virtual void lookup_signed(const MatrixFr& points,
                const MatrixFr& face_normals,
                const MatrixFr& vertex_normals,
                const MatrixFr& edge_normals,
                const VectorI& edge_map,
                VectorF& signed_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points,
                MatrixFr& closest_face_normals) const override {
          igl::signed_distance_pseudonormal(points, m_vertices, m_faces, m_tree,
              face_normals, vertex_normals, edge_normals, edge_map,
              signed_distances, closest_faces, closest_points, closest_face_normals);
        }

    private:
        Tree m_tree;
};

}
}

#endif
