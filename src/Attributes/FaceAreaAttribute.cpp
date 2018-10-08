/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceAreaAttribute.h"

#include <cassert>
#include <iostream>
#include <tbb/tbb.h>

#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void FaceAreaAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& areas = m_values;
    areas = VectorF::Zero(num_faces);
    const auto& vertices = mesh.get_vertices();
    const auto& faces = mesh.get_faces();

    auto compute_2D_triangle_area = [&vertices,&faces](size_t i) {
        const auto& face = faces.segment<3>(i*3);
        const auto& v0 = vertices.segment<2>(face[0]*2);
        const auto& v1 = vertices.segment<2>(face[1]*2);
        const auto& v2 = vertices.segment<2>(face[2]*2);

        return (v0[0]*v1[1] + v1[0]*v2[1] + v2[0]*v0[1]
               -v0[1]*v1[0] - v1[1]*v2[0] - v2[1]*v0[0]) * 0.5;
    };
    auto compute_3D_triangle_area = [&vertices,&faces](size_t i) {
        const auto& face = faces.segment<3>(i*3);
        const auto& v0 = vertices.segment<3>(face[0]*3);
        const auto& v1 = vertices.segment<3>(face[1]*3);
        const auto& v2 = vertices.segment<3>(face[2]*3);

        const auto a = (v1-v0).norm();
        const auto b = (v2-v1).norm();
        const auto c = (v0-v2).norm();
        return 0.25 * sqrt(std::max(0.0, (a+b+c) * (-a+b+c) * (a-b+c) * (a+b-c)));
    };
    auto compute_2D_quad_area = [&vertices, &faces](size_t i) {
        const auto& face = faces.segment<4>(i*4);
        const auto& v0 = vertices.segment<2>(face[0]*2);
        const auto& v1 = vertices.segment<2>(face[1]*2);
        const auto& v2 = vertices.segment<2>(face[2]*2);
        const auto& v3 = vertices.segment<2>(face[3]*2);
        const Vector2F c = (v0 + v1 + v2 + v3) * 0.25;

        return (c[0]*v1[1] + v1[0]*v2[1] + v2[0]*c[1]
               -c[1]*v1[0] - v1[1]*v2[0] - v2[1]*c[0] +
                c[0]*v2[1] + v2[0]*v3[1] + v3[0]*c[1]
               -c[1]*v2[0] - v2[1]*v3[0] - v3[1]*c[0] +
                c[0]*v3[1] + v3[0]*v0[1] + v0[0]*c[1]
               -c[1]*v3[0] - v3[1]*v0[0] - v0[1]*c[0] +
                c[0]*v0[1] + v0[0]*v1[1] + v1[0]*c[1]
               -c[1]*v0[0] - v0[1]*v1[0] - v1[1]*c[0]) * 0.5;
    };
    auto compute_3D_quad_area = [&vertices, &faces](size_t i) {
        const auto& face = faces.segment<4>(i*4);
        const auto& v0 = vertices.segment<3>(face[0]*3);
        const auto& v1 = vertices.segment<3>(face[1]*3);
        const auto& v2 = vertices.segment<3>(face[2]*3);
        const auto& v3 = vertices.segment<3>(face[3]*3);
        const Vector3F c = (v0 + v1 + v2 + v3) * 0.25;

        return (((v2 - c).cross(v1 - c)).norm() +
                ((v3 - c).cross(v2 - c)).norm() +
                ((v0 - c).cross(v3 - c)).norm() +
                ((v1 - c).cross(v0 - c)).norm()) * 0.5;
    };

    if (num_vertex_per_face == 3) {
        if (dim == 2) {
            tbb::parallel_for(tbb::blocked_range<size_t>(0, num_faces),
                    [&compute_2D_triangle_area, &areas](
                            const tbb::blocked_range<size_t>& r) {
                        for (size_t i=r.begin(); i<r.end(); i++) {
                            areas[i] = compute_2D_triangle_area(i);
                        }
                    });
        } else if (dim == 3) {
            tbb::parallel_for(tbb::blocked_range<size_t>(0, num_faces),
                    [&compute_3D_triangle_area, &areas](
                            const tbb::blocked_range<size_t>& r) {
                        for (size_t i=r.begin(); i<r.end(); i++) {
                            areas[i] = compute_3D_triangle_area(i);
                        }
                    }
            );
        } else {
            throw RuntimeError("Unsupported dim=" + std::to_string(dim));
        }
    } else if (num_vertex_per_face == 4) {
        if (dim == 2) {
            tbb::parallel_for(tbb::blocked_range<size_t>(0, num_faces),
                    [&compute_2D_quad_area, &areas](
                            const tbb::blocked_range<size_t>& r) {
                        for (size_t i=r.begin(); i<r.end(); i++) {
                            areas[i] = compute_2D_quad_area(i);
                        }
                    });
        } else if (dim == 3) {
            tbb::parallel_for(tbb::blocked_range<size_t>(0, num_faces),
                    [&compute_3D_quad_area, &areas](
                            const tbb::blocked_range<size_t>& r) {
                        for (size_t i=r.begin(); i<r.end(); i++) {
                            areas[i] = compute_3D_quad_area(i);
                        }
                    }
            );
        } else {
            throw RuntimeError("Unsupported dim=" + std::to_string(dim));
        }
    } else {
        std::cerr << "Face area attribute with " << num_vertex_per_face
            << " vertices per face is not supported yet." << std::endl;
        throw NotImplementedError("Only triangle faces are supported.");
        return;
    }
    assert(areas.array().isFinite().all());
}

