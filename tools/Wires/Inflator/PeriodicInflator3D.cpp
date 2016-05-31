/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PeriodicInflator3D.h"

#include <cmath>
#include <limits>
#include <list>
#include <iostream>
#include <vector>

#include <Boolean/BooleanEngine.h>
#include <Math/ZSparseMatrix.h>
#include <MeshFactory.h>

namespace TriBox3 {
extern "C" {
#include <Misc/tribox3.h>
}
}

#include "PeriodicBoundaryRemesher.h"
#include <CGAL/AABBTree.h>
#include <Wires/Misc/DistanceComputation.h>
#include <Wires/Misc/BoxChecker.h>

using namespace PyMesh;

namespace PeriodicInflator3DHelper {
    void create_box(const VectorF& bbox_min, const VectorF& bbox_max,
            MatrixFr& box_vertices, MatrixIr& box_faces) {
        box_vertices.resize(8, 3);
        box_faces.resize(12, 3);
        box_vertices << bbox_min[0], bbox_min[1], bbox_min[2],
                        bbox_max[0], bbox_min[1], bbox_min[2],
                        bbox_max[0], bbox_max[1], bbox_min[2],
                        bbox_min[0], bbox_max[1], bbox_min[2],
                        bbox_min[0], bbox_min[1], bbox_max[2],
                        bbox_max[0], bbox_min[1], bbox_max[2],
                        bbox_max[0], bbox_max[1], bbox_max[2],
                        bbox_min[0], bbox_max[1], bbox_max[2];
        box_faces << 1, 2, 5,
                     5, 2, 6,
                     3, 4, 7,
                     3, 0, 4,
                     2, 3, 6,
                     3, 7, 6,
                     0, 1, 5,
                     0, 5, 4,
                     4, 5, 6,
                     4, 6, 7,
                     0, 3, 2,
                     0, 2, 1;
    }
}

using namespace PeriodicInflator3DHelper;

void PeriodicInflator3D::clip_to_center_cell() {
    clip_phantom_mesh();
    periodic_remesh();
    update_face_sources();
}

void PeriodicInflator3D::clip_phantom_mesh() {
    VectorF& bbox_min = m_center_cell_bbox_min;
    VectorF& bbox_max = m_center_cell_bbox_max;
    get_center_cell_bbox(bbox_min, bbox_max);

    MatrixFr box_vertices;
    MatrixIr box_faces;
    create_box(bbox_min, bbox_max, box_vertices, box_faces);

    BooleanEngine::Ptr boolean_engine = BooleanEngine::create("cork");
    boolean_engine->set_mesh_1(m_phantom_vertices, m_phantom_faces);
    boolean_engine->set_mesh_2(box_vertices, box_faces);
    boolean_engine->compute_intersection();

    m_vertices = boolean_engine->get_vertices();
    m_faces = boolean_engine->get_faces();
}

void PeriodicInflator3D::update_face_sources() {
    BoxChecker box_checker(m_center_cell_bbox_min, m_center_cell_bbox_max);
    const size_t num_faces = m_faces.rows();
    MatrixFr face_centroids(num_faces, 3);
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = m_faces.row(i);
        face_centroids.row(i) = (
                m_vertices.row(f[0]) +
                m_vertices.row(f[1]) +
                m_vertices.row(f[2])) / 3.0;
    }

    VectorF squared_dists;
    VectorI closest_face_indices;
    m_tree->look_up(face_centroids, squared_dists, closest_face_indices);

    m_face_sources = VectorI::Zero(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = m_faces.row(i);
        const VectorF& centroid = face_centroids.row(i);
        if (box_checker.is_on_boundary(centroid)) {
            continue;
        } else {
            m_face_sources[i] = m_phantom_face_sources[closest_face_indices[i]];
        }
    }
}

void PeriodicInflator3D::periodic_remesh() {
    Float default_thickness = m_parameter_manager->get_default_thickness();
    default_thickness *= pow(0.5, m_subdiv_order);
    PeriodicBoundaryRemesher remesher(m_vertices, m_faces,
            m_center_cell_bbox_min,
            m_center_cell_bbox_max);
    remesher.remesh(default_thickness);
    m_vertices = remesher.get_vertices();
    m_faces = remesher.get_faces();
}

