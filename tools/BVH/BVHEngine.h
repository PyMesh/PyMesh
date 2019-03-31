/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Mesh.h>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

/**
 * Boundary Volume Hierarchy is commonly used for accelerate spatial queries and
 * intersections tests.  This class unify multiple BVH implementations under the
 * same interface.
 */
class BVHEngine {
    public:
        typedef std::shared_ptr<BVHEngine> Ptr;
        static Ptr create(const std::string& engine_name, size_t dim);
        static std::vector<std::string> get_available_engines();

    public:
        virtual ~BVHEngine() = default;

    public:
        void set_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
            if (faces.cols() != 3) {
                throw NotImplementedError(
                        "Only Triangle mesh is supported by BVH engine");
            }
            m_vertices = vertices;
            m_faces = faces;
        }

        /**
         * Build BVH for the given mesh.
         */
        virtual void build() {
            throw NotImplementedError("BVH algorithm is not implemented");
        }

        /**
         * For each point in points, lookup the closest points on mesh and the
         * corresponding distances and faces.
         */
        virtual void lookup(const MatrixFr& points,
                VectorF& squared_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points) const {
            throw NotImplementedError("BVH algorithm is not implemented");
        }

        /**
         * For each point in points, lookup the closest points on mesh and the
         * corresponding signed (un-squared) distances and faces.
         * Warning: only work with IGL engine
         */
        virtual void lookup_signed(const MatrixFr& points,
                const MatrixFr& face_normals,
                const MatrixFr& vertex_normals,
                const MatrixFr& edge_normals,
                const VectorI& edge_map,
                VectorF& signed_distances,
                VectorI& closest_faces,
                MatrixFr& closest_points,
                MatrixFr& closest_face_normals) const {
            throw NotImplementedError("BVH algorithm is not implemented");
        }

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
};

}
