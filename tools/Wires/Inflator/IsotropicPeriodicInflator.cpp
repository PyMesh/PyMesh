#include "IsotropicPeriodicInflator.h"

#include <functional>
#include <iostream>
#include <sstream>

#include <Boolean/BooleanEngine.h>
#include <Math/MatrixUtils.h>
#include <MeshUtils/Boundary.h>
#include <MeshUtils/SubMesh.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/ObtuseTriangleRemoval.h>
#include <tetgen/TetgenWrapper.h>
#include <Wires/Misc/BoundaryRemesher.h>
#include <Wires/Misc/BoxChecker.h>

#include "AABBTree.h"

namespace IsotropicPeriodicInflator3DHelper {
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

using namespace IsotropicPeriodicInflator3DHelper;

IsotropicPeriodicInflator::IsotropicPeriodicInflator(
        WireNetwork::Ptr wire_network) : PeriodicInflator(wire_network) {
    if (wire_network->get_dim() != 3) {
        throw RuntimeError(
                "IsotropicPeriodicInflator only support 3D geometry.");
    }
}

void IsotropicPeriodicInflator::clip_to_center_cell() {
    initialize_center_cell_and_octa_cell();
    clip_phantom_mesh_with_octa_cell();
    remesh_boundary();
    reflect();
    update_face_sources();
}

void IsotropicPeriodicInflator::initialize_center_cell_and_octa_cell() {
    VectorF& bbox_min = m_center_cell_bbox_min;
    VectorF& bbox_max = m_center_cell_bbox_max;
    get_center_cell_bbox(bbox_min, bbox_max);
    VectorF bbox_center = 0.5 * (bbox_min + bbox_max);

    m_octa_cell_bbox_min = bbox_center;
    m_octa_cell_bbox_max = bbox_max;

    auto negate_col = [=](MatrixFr& M, size_t col) {
        const size_t num_rows = M.rows();
        for (size_t i=0; i<num_rows; i++) {
            M(i, col) *= -1;
        }
    };

    m_reflection_operators.push_back([=](const MatrixFr& vts) {
            MatrixFr vertices = vts.rowwise() - bbox_center.transpose();
            negate_col(vertices, 0);
            vertices.rowwise() += bbox_center.transpose();
            return vertices; } );
    m_reflection_operators.push_back([=](const MatrixFr& vts) {
            MatrixFr vertices = vts.rowwise() - bbox_center.transpose();
            negate_col(vertices, 1);
            vertices.rowwise() += bbox_center.transpose();
            return vertices; } );
    m_reflection_operators.push_back([=](const MatrixFr& vts) {
            MatrixFr vertices = vts.rowwise() - bbox_center.transpose();
            negate_col(vertices, 2);
            vertices.rowwise() += bbox_center.transpose();
            return vertices; } );
}

void IsotropicPeriodicInflator::clip_phantom_mesh_with_octa_cell() {
    MatrixFr box_vertices;
    MatrixIr box_faces;
    create_box(m_octa_cell_bbox_min, m_octa_cell_bbox_max,
            box_vertices, box_faces);

    BooleanEngine::Ptr boolean_engine = BooleanEngine::create("igl");
    boolean_engine->set_mesh_1(m_phantom_vertices, m_phantom_faces);
    boolean_engine->set_mesh_2(box_vertices, box_faces);
    boolean_engine->compute_intersection();

    m_vertices = boolean_engine->get_vertices();
    m_faces = boolean_engine->get_faces();

#ifndef NDEBUG
    save_mesh("clipped.msh", m_vertices, m_faces);
    save_mesh("phantom.msh", m_phantom_vertices, m_phantom_faces);
    save_mesh("clip_box.msh",box_vertices, box_faces);
#endif
}

void IsotropicPeriodicInflator::remesh_boundary() {
    typedef std::function<bool(const VectorF&)> IndicatorFunc;
    const Float tol = 1e-6;
    std::vector<IndicatorFunc> bd_indicators;
    bd_indicators.push_back([=](const VectorF& v)
            { return fabs(v[0] - m_octa_cell_bbox_max[0]) < tol; });
    bd_indicators.push_back([=](const VectorF& v)
            { return fabs(v[1] - m_octa_cell_bbox_max[1]) < tol; });
    bd_indicators.push_back([=](const VectorF& v)
            { return fabs(v[2] - m_octa_cell_bbox_max[2]) < tol; });

    size_t count = 0;
    for (auto f : bd_indicators) {
        BoundaryRemesher remesher(m_vertices, m_faces);
        remesher.remesh(f, 0.1);
        m_vertices = remesher.get_vertices();
        m_faces = remesher.get_faces();
        assert(m_vertices.rows() > 0);
        assert(m_faces.rows() > 0);

#ifndef NDEBUG
        VectorF debug = VectorF::Zero(m_vertices.rows());
        std::stringstream fname;
        fname << "remeshed_" << count << ".msh";
        save_mesh(fname.str(), m_vertices, m_faces, debug);
#endif
        count++;
    }
}

void IsotropicPeriodicInflator::reflect() {
    TetgenWrapper tetrahedronizer(m_vertices, m_faces);
    tetrahedronizer.run("qpa0.01Q");

    auto vertices = tetrahedronizer.get_vertices();
    auto voxels = tetrahedronizer.get_voxels();

    size_t num_vertices = vertices.rows();

    for (size_t i=0; i<3; i++) {
        MatrixFr r_vts = vertices;
        for (size_t j=0; j<num_vertices; j++) {
            r_vts(j, i) = -r_vts(j, i) + 2 * m_octa_cell_bbox_min[i];
        }

        MatrixIr r_voxels = voxels.array() + num_vertices;
        r_voxels.col(0).swap(r_voxels.col(1));

        vertices = MatrixUtils::vstack<MatrixFr>({vertices, r_vts});
        voxels = MatrixUtils::vstack<MatrixIr>({voxels, r_voxels});
        num_vertices = vertices.rows();
    }

    DuplicatedVertexRemoval remover(vertices, voxels);
    remover.run(1e-12);
    vertices = remover.get_vertices();
    voxels = remover.get_faces();

    Boundary::Ptr bd = Boundary::extract_volume_boundary_raw(vertices, voxels);
    m_vertices = vertices;
    m_faces = bd->get_boundaries();

    remove_isolated_vertices();
}

void IsotropicPeriodicInflator::update_face_sources() {
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

#ifndef NDEBUG
    save_mesh("reflected.msh", m_vertices, m_faces, m_face_sources.cast<Float>());
#endif
}

