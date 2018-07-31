/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <Core/EigenTypedef.h>
#include <BVH/BVHEngine.h>

#include <igl/AABB.h>

namespace PyMesh {
namespace IGL {

class AABBTree : public BVHEngine {
    public:
        using Ptr = std::shared_ptr<AABBTree>;
        using Tree = igl::AABB<MatrixFr, 3>;

    public:
        virtual ~AABBTree() = default;

        virtual void build();

        virtual void lookup(const MatrixFr& points,
                VectorF& squared_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points) const;

    private:
        Tree m_tree;
};

}
}

#endif
