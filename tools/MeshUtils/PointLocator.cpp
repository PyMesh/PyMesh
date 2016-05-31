/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PointLocator.h"

#include <cassert>
#include <limits>
#include <sstream>

#include <Core/Exception.h>
#include <Misc/HashGrid.h>

using namespace PyMesh;

PointLocator::PointLocator(Mesh::Ptr mesh) : m_mesh(mesh) {
    init_elements();
    init_barycentric_solvers();
    init_hash_grid();
}

void PointLocator::locate(const MatrixFr& points) {
    const Float eps = 1e-6;
    const size_t num_pts = points.rows();
    m_voxel_idx = VectorI::Zero(num_pts);
    m_barycentric_coords = MatrixFr::Zero(num_pts, m_vertex_per_element);

    for (size_t i=0; i<num_pts; i++) {
        VectorF v = points.row(i);
        VectorI candidate_elems = m_grid->get_items_near_point(v);

        VectorF barycentric_coord;
        VectorF best_barycentric_coord;

        bool found = false;
        Float least_negative_coordinate = -std::numeric_limits<Float>::max();
        const size_t num_candidates = candidate_elems.size();
        for (size_t j=0; j<num_candidates; j++) {
            barycentric_coord = compute_barycentric_coord(
                    v, candidate_elems[j]);
            Float min_barycentric_coord = barycentric_coord.minCoeff();
            if (min_barycentric_coord > least_negative_coordinate) {
                found = true;
                least_negative_coordinate = min_barycentric_coord;
                m_voxel_idx[i] = candidate_elems[j];
                best_barycentric_coord = barycentric_coord;
                if (min_barycentric_coord >= -eps) {
                    break;
                }
            }
        }

        if (!found) {
            std::stringstream err_msg;
            err_msg << "Point ( ";
            for (size_t i=0; i<m_mesh->get_dim(); i++) {
                err_msg << v[i] << " ";
            }
            err_msg << ") is not inside of any voxels" << std::endl;
            throw RuntimeError(err_msg.str());
        }

        m_barycentric_coords.row(i) = best_barycentric_coord;
    }
}

void PointLocator::clear() {
    m_voxel_idx.resize(0);
    m_barycentric_coords.resize(0,0);
}

void PointLocator::init_elements() {
    const size_t dim = m_mesh->get_dim();

    if (dim == 2) {
        if (m_mesh->get_num_faces() == 0) {
            throw RuntimeError("2D Mesh has no faces.");
        }
        m_elements = m_mesh->get_faces();
        m_vertex_per_element = m_mesh->get_vertex_per_face();
        if (m_vertex_per_element != 3) {
            throw NotImplementedError(
                    "Only triangle elements are supported in 2D");
        }
    } else if (dim == 3) {
        if (m_mesh->get_num_voxels() == 0) {
            throw RuntimeError("3D Mesh has no voxels.");
        }
        m_elements = m_mesh->get_voxels();
        m_vertex_per_element = m_mesh->get_vertex_per_voxel();
        if (m_vertex_per_element != 4) {
            throw NotImplementedError(
                    "Only tetrahedron elements are supported in 2D");
        }
    } else {
        throw NotImplementedError("Only 2D and 3D mesh are supported");
    }
}

void PointLocator::init_barycentric_solvers() {
    const size_t dim = m_mesh->get_dim();

    const size_t num_elements = m_elements.size() / m_vertex_per_element;
    m_barycentric_solvers = MatrixF::Zero(num_elements*dim, dim);
    m_last_vertices = MatrixF::Zero(num_elements, dim);

    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = m_elements.segment(i*m_vertex_per_element, m_vertex_per_element);
        VectorF last_v = m_mesh->get_vertex(elem[dim]);
        MatrixFr M(dim, dim);
        for (size_t j=0; j<dim; j++) {
            M.row(j) = m_mesh->get_vertex(elem[j]) - last_v;
        }
        m_barycentric_solvers.block(i*dim, 0, dim, dim) = M.transpose().inverse();
        m_last_vertices.row(i) = last_v;
    }
}

void PointLocator::init_hash_grid() {
    const size_t dim = m_mesh->get_dim();
    const size_t num_elements = m_elements.size() / m_vertex_per_element;

    Float cell_size = compute_cell_size();
    m_grid = HashGrid::create(cell_size, dim);

    MatrixF elem_vtx(m_vertex_per_element, dim);
    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = m_elements.segment(i*m_vertex_per_element, m_vertex_per_element);
        for (size_t j=0; j<m_vertex_per_element; j++) {
            elem_vtx.row(j) = m_mesh->get_vertex(elem[j]);
        }
        m_grid->insert_bbox(i, elem_vtx);
    }
}

Float PointLocator::compute_cell_size() const {
    const size_t dim = m_mesh->get_dim();
    const size_t num_elements = m_elements.size() / m_vertex_per_element;

    Float total_edge_len = 0.0;

    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = m_elements.segment(i*m_vertex_per_element, m_vertex_per_element);
        for (size_t j=0; j<m_vertex_per_element; j++) {
            Float edge_len = (
                    m_mesh->get_vertex(elem[j]) -
                    m_mesh->get_vertex(elem[(j+1)%m_vertex_per_element])).norm();
            total_edge_len += edge_len;
        }
    }

    Float ave_edge_len = total_edge_len / (num_elements * m_vertex_per_element);
    return 0.1 * ave_edge_len;
}

VectorF PointLocator::compute_barycentric_coord(const VectorF& v,
        size_t elem_idx) {
    const size_t dim = m_mesh->get_dim();
    MatrixF solver = m_barycentric_solvers.block(elem_idx*dim, 0, dim, dim);
    VectorF last_v = m_last_vertices.row(elem_idx);
    VectorF sol = solver * (v - last_v);
    VectorF barycentric_coord(m_vertex_per_element);
    barycentric_coord.segment(0, dim) = sol;
    barycentric_coord[dim] = 1.0 - sol.sum();
    return barycentric_coord;
}

