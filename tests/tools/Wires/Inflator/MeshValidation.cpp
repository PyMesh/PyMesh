/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshValidation.h"

#include <cassert>
#include <iostream>
#include <Misc/HashGrid.h>
#include <Misc/TripletMap.h>

namespace MeshValidationHelper {
    typedef TripletMap<size_t> EdgeMap;
    EdgeMap compute_edge_map(const MatrixIr& faces) {
        assert(faces.cols() == 3);
        EdgeMap result;
        const size_t num_faces = faces.rows();
        for (size_t i=0; i<num_faces; i++) {
            const Vector3I& f = faces.row(i);
            Triplet e0(f[1], f[2]);
            Triplet e1(f[2], f[0]);
            Triplet e2(f[0], f[1]);

            result.insert(e0, i);
            result.insert(e1, i);
            result.insert(e2, i);
        }

        return result;
    }

    HashGrid::Ptr compute_vertex_grid(const MatrixFr& vertices, Float cell_size) {
        const size_t dim = vertices.cols();
        const size_t num_vertices = vertices.rows();
        HashGrid::Ptr grid = HashGrid::create(cell_size, dim);
        for (size_t i=0; i<num_vertices; i++) {
            const VectorF& v = vertices.row(i);
            grid->insert(i, v);
        }
        return grid;
    }

    bool match(HashGrid::Ptr grid, const Vector3F& v) {
        VectorI candidates = grid->get_items_near_point(v);
        if (candidates.size() != 1) {
            std::cout << "<" << v.transpose() << "> : " << candidates.transpose()
                << std::endl;
        }
        return candidates.size() > 0;
    }
}

using namespace MeshValidationHelper;

bool MeshValidation::is_water_tight(
        const MatrixFr& vertices, const MatrixIr& faces) {
    EdgeMap edge_map = compute_edge_map(faces);
    bool result = true;
    for (auto itr : edge_map) {
        result = result && (itr.second.size() > 1);
    }
    return result;
}

bool MeshValidation::is_manifold(
        const MatrixFr& vertices, const MatrixIr& faces) {
    EdgeMap edge_map = compute_edge_map(faces);
    bool result = true;
    for (auto itr : edge_map) {
        result = result && (itr.second.size() == 2);
    }
    return result;
}

bool MeshValidation::is_periodic(
        const MatrixFr& vertices, const MatrixIr& faces) {
    const Float EPS = 1e-6;
    HashGrid::Ptr grid = compute_vertex_grid(vertices, EPS);

    Vector3F bbox_min = vertices.colwise().minCoeff();
    Vector3F bbox_max = vertices.colwise().maxCoeff();
    Vector3F bbox_size = bbox_max - bbox_min;

    Vector3F offsets[] = {
        Vector3F( bbox_size[0], 0.0, 0.0),
        Vector3F(-bbox_size[0], 0.0, 0.0),
        Vector3F(0.0, bbox_size[1], 0.0),
        Vector3F(0.0,-bbox_size[1], 0.0),
        Vector3F(0.0, 0.0, bbox_size[2]),
        Vector3F(0.0, 0.0,-bbox_size[2])
    };

    bool result = true;
    const size_t num_vertices = vertices.rows();
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = vertices.row(i);
        if (fabs(v[0] - bbox_min[0]) < EPS) {
            result = result && match(grid, v + offsets[0]);
        }
        if (fabs(v[0] - bbox_max[0]) < EPS) {
            result = result && match(grid, v + offsets[1]);
        }
        if (fabs(v[1] - bbox_min[1]) < EPS) {
            result = result && match(grid, v + offsets[2]);
        }
        if (fabs(v[1] - bbox_max[1]) < EPS) {
            result = result && match(grid, v + offsets[3]);
        }
        if (fabs(v[2] - bbox_min[2]) < EPS) {
            result = result && match(grid, v + offsets[4]);
        }
        if (fabs(v[2] - bbox_max[2]) < EPS) {
            result = result && match(grid, v + offsets[5]);
        }
    }
    return result;
}

bool MeshValidation::face_source_is_valid(
        const MatrixFr& vertices,
        const MatrixIr& faces,
        const VectorI& face_sources) {
    const Float EPS = 1e-6;
    const size_t num_vertices = vertices.rows();
    const size_t num_faces = faces.rows();

    Vector3F bbox_min = vertices.colwise().minCoeff();
    Vector3F bbox_max = vertices.colwise().maxCoeff();

    bool result = true;
    for (size_t i=0; i<num_faces; i++) {
        const Vector3I& f = faces.row(i);
        const Vector3F& v0 = vertices.row(f[0]);
        const Vector3F& v1 = vertices.row(f[1]);
        const Vector3F& v2 = vertices.row(f[2]);

        if (fabs(v0[0] - bbox_min[0]) < EPS &&
            fabs(v1[0] - bbox_min[0]) < EPS &&
            fabs(v2[0] - bbox_min[0]) < EPS) {
            result = result && (face_sources[i] == 0);
            continue;
        }

        if (fabs(v0[1] - bbox_min[1]) < EPS &&
            fabs(v1[1] - bbox_min[1]) < EPS &&
            fabs(v2[1] - bbox_min[1]) < EPS) {
            result = result && (face_sources[i] == 0);
            continue;
        }

        if (fabs(v0[2] - bbox_min[2]) < EPS &&
            fabs(v1[2] - bbox_min[2]) < EPS &&
            fabs(v2[2] - bbox_min[2]) < EPS) {
            result = result && (face_sources[i] == 0);
            continue;
        }

        if (fabs(v0[0] - bbox_max[0]) < EPS &&
            fabs(v1[0] - bbox_max[0]) < EPS &&
            fabs(v2[0] - bbox_max[0]) < EPS) {
            result = result && (face_sources[i] == 0);
            continue;
        }

        if (fabs(v0[1] - bbox_max[1]) < EPS &&
            fabs(v1[1] - bbox_max[1]) < EPS &&
            fabs(v2[1] - bbox_max[1]) < EPS) {
            result = result && (face_sources[i] == 0);
            continue;
        }

        if (fabs(v0[2] - bbox_max[2]) < EPS &&
            fabs(v1[2] - bbox_max[2]) < EPS &&
            fabs(v2[2] - bbox_max[2]) < EPS) {
            result = result && (face_sources[i] == 0);
            continue;
        }
        result = result && (face_sources[i] != 0);
        if (!result) {
            std::cout << i << ":  ";
            std::cout << face_sources[i] << std::endl;
            std::cout << v0.transpose() << std::endl;
            std::cout << v1.transpose() << std::endl;
            std::cout << v2.transpose() << std::endl;
            return result;
        }
    }
    return result;
}

