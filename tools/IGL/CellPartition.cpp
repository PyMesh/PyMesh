/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include "CellPartition.h"
#include <Math/MatrixUtils.h>
#include <igl/copyleft/cgal/extract_cells.h>
#include <igl/copyleft/cgal/propagate_winding_numbers.h>
#include <igl/copyleft/cgal/RemeshSelfIntersectionsParam.h>
#include <igl/copyleft/cgal/SelfIntersectMesh.h>
#include <igl/extract_manifold_patches.h>
#include <igl/remove_unreferenced.h>
#include <igl/unique_edge_map.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

using namespace PyMesh;

CellPartition::Ptr CellPartition::create(const Mesh::Ptr& mesh) {
    const MatrixFr vertices = MatrixUtils::reshape<MatrixFr>(
            mesh->get_vertices(), mesh->get_num_vertices(), mesh->get_dim());
    const MatrixIr faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(), mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    return CellPartition::Ptr(new CellPartition(vertices, faces));
}

CellPartition::Ptr CellPartition::create_raw(
        const MatrixFr& vertices, const MatrixIr& faces) {
    return CellPartition::Ptr(new CellPartition(vertices, faces));
}

void CellPartition::run() {
    typedef CGAL::Epeck Kernel;
    typedef Kernel::FT ExactScalar;
    typedef Eigen::Matrix<ExactScalar,
            Eigen::Dynamic,Eigen::Dynamic, Eigen::RowMajor> MatrixEr;

    // Resolve self intersection
    igl::copyleft::cgal::RemeshSelfIntersectionsParam params;

    MatrixEr resolved_vertices;
    MatrixIr resolved_faces;
    {
        MatrixEr V;
        MatrixIr F;
        MatrixIr intersecting_faces;
        VectorI source_vertices;
        igl::copyleft::cgal::SelfIntersectMesh<
            Kernel,
            MatrixFr, MatrixIr,
            MatrixEr, MatrixIr,
            MatrixIr, VectorI, VectorI> resolver(
                    m_vertices, m_faces,
                    params,
                    V, F,
                    intersecting_faces,
                    m_source_faces, source_vertices);

        // Merge coinciding vertices into non-manifold vertices.
        std::for_each(F.data(), F.data()+F.size(),
                [&source_vertices](typename MatrixIr::Scalar& a) {
                a=source_vertices[a];
                });

        // Remove unreferenced vertices.
        Eigen::VectorXi UIM;
        igl::remove_unreferenced(V, F, resolved_vertices, resolved_faces, UIM);
    }

    // Build edge map
    Eigen::MatrixXi E, uE;
    Eigen::VectorXi EMAP;
    std::vector<std::vector<size_t> > uE2E;
    igl::unique_edge_map(resolved_faces, E, uE, EMAP, uE2E);

    // patches
    const size_t num_patches = igl::extract_manifold_patches(
            resolved_faces, EMAP, uE2E, m_patches);

    // cells
    const size_t num_cells = igl::copyleft::cgal::extract_cells(
            resolved_vertices, resolved_faces,
            m_patches, E, uE, uE2E, EMAP, m_cells);
    assert(m_cells.rows() == num_patches);
    assert(m_cells.cols() == 2);

    VectorI labels = VectorI::Zero(resolved_faces.rows());
    // winding numbers
    igl::copyleft::cgal::propagate_winding_numbers(
            resolved_vertices, resolved_faces,
            uE, uE2E, num_patches, m_patches, num_cells, m_cells,
            labels, m_winding_number);

    // Cast resolved mesh back to Float
    m_vertices = MatrixFr(resolved_vertices.rows(), resolved_vertices.cols());
    std::transform(resolved_vertices.data(),
            resolved_vertices.data() + resolved_vertices.size(),
            m_vertices.data(), [](const ExactScalar& val){
                return CGAL::to_double(val);
            });
    m_faces = resolved_faces;
}

size_t CellPartition::get_num_cells() const {
    if (m_cells.rows() > 0) return m_cells.maxCoeff();
    else return 0;
}

MatrixIr CellPartition::get_cell_faces(const size_t cell_id) const {
    const size_t num_faces = m_faces.rows();
    std::vector<VectorI> cell_faces;
    for (size_t i=0; i<num_faces; i++) {
        const size_t patch_id = m_patches[i];
        if (m_cells(patch_id,0) == cell_id) {
            cell_faces.push_back(m_faces.row(i).reverse());
        } else if (m_cells(patch_id,1) == cell_id) {
            cell_faces.push_back(m_faces.row(i));
        }
    }
    MatrixIr faces = MatrixUtils::rowstack(cell_faces);
    return faces;
}

size_t CellPartition::get_num_patches() const {
    if (m_patches.size() > 0) return m_patches.maxCoeff();
    else return 0;
}
