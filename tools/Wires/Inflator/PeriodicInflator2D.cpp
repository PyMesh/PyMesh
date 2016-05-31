/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PeriodicInflator2D.h"
#include <Misc/TriBox2D.h>
#include <Boolean/BooleanEngine.h>

#include <iostream>
#include <list>

using namespace PyMesh;

namespace PeriodicInflator2DHelper {
    enum Location { INSIDE, ON_BORDER, OUTSIDE };
    Location bbox_check(
            const Vector2F& bbox_min,
            const Vector2F& bbox_max,
            const Vector2F& p0,
            const Vector2F& p1,
            const Vector2F& p2) {
        Vector2F bbox_center = 0.5 * (bbox_min + bbox_max);
        Vector2F bbox_size = 0.5 * (bbox_max - bbox_min);

        bool inside = (
                (p0.array() < bbox_max.array()).all() &&
                (p0.array() > bbox_min.array()).all() &&
                (p1.array() < bbox_max.array()).all() &&
                (p1.array() > bbox_min.array()).all() &&
                (p2.array() < bbox_max.array()).all() &&
                (p2.array() > bbox_min.array()).all());
        if (inside) return INSIDE;

        const Float tri[3][2] = {
            {p0[0], p0[1]},  {p1[0], p1[1]},  {p2[0], p2[1]}
        };
        bool overlap = TriBox2D::triBoxOverlap(
                bbox_center.data(),
                bbox_size.data(),
                tri);
        if (!overlap) return OUTSIDE;
        else return ON_BORDER;
    }

    void cut_face(
            const Vector2F& bbox_min,
            const Vector2F& bbox_max,
            const Vector2F& p0,
            const Vector2F& p1,
            const Vector2F& p2,
            MatrixFr& vertices, MatrixIr& faces) {
        MatrixFr box_vertices(4,2);
        box_vertices << bbox_min[0], bbox_min[1],
                        bbox_max[0], bbox_min[1],
                        bbox_max[0], bbox_max[1],
                        bbox_min[0], bbox_max[1];
        MatrixIr box_faces(2, 3);
        box_faces << 0, 1, 2,
                     0, 2, 3;

        MatrixFr tri_vertices(3, 2);
        tri_vertices << p0.transpose(),
                        p1.transpose(),
                        p2.transpose();
        MatrixIr tri_faces(1, 3);
        tri_faces << 0, 1, 2;

        BooleanEngine::Ptr boolean_engine = BooleanEngine::create("clipper");
        boolean_engine->set_mesh_1(tri_vertices, tri_faces);
        boolean_engine->set_mesh_2(box_vertices, box_faces);
        boolean_engine->compute_intersection();
        vertices = boolean_engine->get_vertices();
        faces = boolean_engine->get_faces();
    }

    template<typename M, typename T>
    void append(const M& vertices, std::list<T>& flattened) {
        const size_t rows = vertices.rows();
        const size_t cols = vertices.cols();
        for (size_t i=0; i<rows; i++) {
            for (size_t j=0; j<cols; j++) {
                flattened.push_back(vertices.coeff(i,j));
            }
        }
    }
}

using namespace PeriodicInflator2DHelper;

void PeriodicInflator2D::clip_to_center_cell() {
    assert(m_phantom_faces.rows() > 0);
    std::list<Float> vertices;
    std::list<size_t> faces;
    std::list<int> face_sources;

    append(m_phantom_vertices, vertices);
    clip_phantom_mesh(vertices, faces, face_sources);

    assert(vertices.size() % 2 == 0);
    assert(faces.size() % 3 == 0);
    size_t num_vertices = vertices.size() / 2;
    size_t num_faces = faces.size() / 3;
    assert(face_sources.size() == num_faces);

    m_vertices.resize(num_vertices, 2);
    m_faces.resize(num_faces, 3);
    m_face_sources.resize(num_faces);
    std::copy(vertices.begin(), vertices.end(), m_vertices.data());
    std::copy(faces.begin(), faces.end(), m_faces.data());
    std::copy(face_sources.begin(), face_sources.end(), m_face_sources.data());
}

void PeriodicInflator2D::clip_phantom_mesh(
        std::list<Float>& vertices, std::list<size_t>& faces,
        std::list<int>& face_sources) {
    VectorF bbox_min, bbox_max;
    get_center_cell_bbox(bbox_min, bbox_max);
    const size_t dim = 2;
    const size_t num_phantom_faces = m_phantom_faces.rows();
    size_t base_index = m_phantom_vertices.rows();
    for (size_t i=0; i<num_phantom_faces; i++) {
        const int face_source = m_phantom_face_sources[i];
        const Vector3I& face = m_phantom_faces.row(i);
        const Vector2F& p0 = m_phantom_vertices.row(face[0]);
        const Vector2F& p1 = m_phantom_vertices.row(face[1]);
        const Vector2F& p2 = m_phantom_vertices.row(face[2]);

        Location loc = bbox_check(bbox_min, bbox_max, p0, p1, p2);
        if (loc == INSIDE) {
            append(face, faces);
            face_sources.push_back(face_source);
        } else if (loc == ON_BORDER) {
            MatrixFr cut_vertices;
            MatrixIr cut_faces;
            cut_face(bbox_min, bbox_max, p0, p1, p2,
                    cut_vertices, cut_faces);

            cut_faces.array() += base_index;
            append(cut_vertices, vertices);
            append(cut_faces, faces);
            const size_t num_cut_faces = cut_faces.rows();
            for (size_t j=0; j<num_cut_faces; j++)
                face_sources.push_back(face_source);
            base_index += cut_vertices.rows();
        }
    }
}




