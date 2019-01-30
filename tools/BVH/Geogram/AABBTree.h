/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_GEOGRAM

#include <Core/EigenTypedef.h>
#include <BVH/BVHEngine.h>
#include <Geogram/GeogramBase.h>

#include <geogram/mesh/mesh.h>
#include <geogram/mesh/mesh_AABB.h>

#include <memory>

namespace PyMesh {
namespace Geogram {

class AABBTree : public BVHEngine, GeogramBase {
    public:
        using Ptr = std::shared_ptr<AABBTree>;
        using Tree = GEO::MeshFacetsAABB;
        using TreePtr = std::shared_ptr<Tree>;

    public:
        virtual ~AABBTree() = default;

        virtual void build();

        virtual void lookup(const MatrixFr& points,
                VectorF& squared_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points) const;
    private:
        TreePtr m_tree;
        GeoMeshPtr m_geo_mesh;
};

}
}

#endif
