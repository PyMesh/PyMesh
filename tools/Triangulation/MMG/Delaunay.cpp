/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_MMG

#include "Delaunay.h"
#include <Core/Exception.h>

extern "C" {
#include <mmg/libmmg.h>
}

#include <iostream>

using namespace PyMesh;

void MMG::Delaunay::run() {
    MMG5_pMesh mmgMesh;
    MMG5_pSol mmgSol;

    mmgMesh = NULL;
    mmgSol  = NULL;
    if (MMG2D_Init_mesh(MMG5_ARG_start,
                MMG5_ARG_ppMesh, &mmgMesh,
                MMG5_ARG_ppMet, &mmgSol,
                MMG5_ARG_end) != 1) {
        throw RuntimeError("MMG2D_Init_mesh failed.");
    }

    const Vector2F bbox_min = m_points.colwise().minCoeff();
    const Vector2F bbox_max = m_points.colwise().maxCoeff();
    const Float tol = (bbox_max - bbox_min).norm() / 20.0;

    if (MMG2D_Set_solSize(mmgMesh, mmgSol, MMG5_Vertex, 0, MMG5_Scalar) != 1) {
        throw RuntimeError("MMG2D_Set_solSize failed.");
    }

    if (MMG2D_Set_iparameter(mmgMesh, mmgSol, MMG2D_IPARAM_verbose, 0) != 1) {
        throw RuntimeError("Setting MMG2D_IPARAM_verbose failed.");
    }
    if (MMG2D_Set_dparameter(mmgMesh, mmgSol, MMG2D_DPARAM_hmax, tol) != 1) {
        throw RuntimeError("Setting MMG2D_DPARAM_hmax failed.");
    }

    const size_t num_points = m_points.rows();
    const size_t num_segments = m_segments.rows();
    if (MMG2D_Set_meshSize(mmgMesh,num_points,0,num_segments) != 1) {
        throw RuntimeError("MMG2D_Set_meshSize failed.");
    }

    for (size_t i=0; i<num_points; i++) {
        if (MMG2D_Set_vertex(mmgMesh, m_points(i,0), m_points(i, 1), i+1, i+1) != 1) {
            throw RuntimeError("MMG2D_Set_vertex failed for vertex "
                    + std::to_string(i) + ".");
        }
    }

    for (size_t i=0; i<num_segments; i++) {
        if (MMG2D_Set_edge(mmgMesh,
                    m_segments(i, 0) + 1,
                    m_segments(i, 1) + 1,
                    i+1, i+1) != 1 )  {
            throw RuntimeError("MMG2D_Set_edge failed for edge "
                    + std::to_string(i) + ".");
        }
    }

    if (MMG2D_Chk_meshData(mmgMesh, mmgSol) != 1) {
        throw RuntimeError("MMG2D_Chk_meshData failed");
    }

    int r = MMG2D_mmg2dmesh(mmgMesh,mmgSol);
    switch (r) {
        case MMG5_SUCCESS:
            break;
        case MMG5_LOWFAILURE:
            std::cerr << "Warning: MMG encountered low failure.  Results may not not be valid."
                << std::endl;
            break;
        case MMG5_STRONGFAILURE:
            throw RuntimeError("MMG strong failure!");
        default:
            throw NotImplementedError("Unknown MMG failure encountered.");
    }

    int num_vertices, num_triangles, num_edges;
    if (MMG2D_Get_meshSize(mmgMesh,
                &num_vertices, &num_triangles, &num_edges) !=1) {
        throw RuntimeError("MMG2D_Get_meshSize failed.");
    }

    m_vertices.resize(num_vertices, 2);
    int ref, is_corner, is_required;
    for (size_t i=0; i<num_vertices; i++) {
        if (MMG2D_Get_vertex(mmgMesh,
                    &m_vertices(i, 0),
                    &m_vertices(i, 1),
                    &ref,
                    &is_corner,
                    &is_required) != 1 ) {
            throw RuntimeError("MMG2D_Get_vertex failed.");
        }
    }

    m_faces.resize(num_triangles, 3);
    for (size_t i=0; i<num_triangles; i++) {
        if (MMG2D_Get_triangle(mmgMesh,
                    &m_faces(i, 0),
                    &m_faces(i, 1),
                    &m_faces(i, 2),
                    &ref,
                    &is_required) != 1 ) {
            throw RuntimeError("MMG2D_Get_triangle failed.");
        }
    }

    // MMG is 1 based unfortunately.
    m_faces.array() -= 1;

    //MMG2D_saveMesh(mmgMesh, "mmg_debug.mesh");

    MMG2D_Free_all(MMG5_ARG_start,
            MMG5_ARG_ppMesh,&mmgMesh,MMG5_ARG_ppMet,&mmgSol,
            MMG5_ARG_end);
}

void MMG::Delaunay::refine(const MatrixFr& metric) {
    const size_t num_input_vertices = m_vertices.rows();
    const size_t num_input_faces = m_faces.rows();

    if (num_input_vertices == 0) {
        return;
    }

    MMG5_pMesh mmgMesh;
    MMG5_pSol mmgSol;

    mmgMesh = NULL;
    mmgSol  = NULL;
    if (MMG2D_Init_mesh(MMG5_ARG_start,
                MMG5_ARG_ppMesh, &mmgMesh,
                MMG5_ARG_ppMet, &mmgSol,
                MMG5_ARG_end) != 1) {
        throw RuntimeError("MMG2D_Init_mesh failed.");
    }

    const Vector2F bbox_min = m_vertices.colwise().minCoeff();
    const Vector2F bbox_max = m_vertices.colwise().maxCoeff();
    const Float tol = (bbox_max - bbox_min).norm() / 20.0;

    if (MMG2D_Set_iparameter(mmgMesh, mmgSol, MMG2D_IPARAM_verbose, 1) != 1) {
        throw RuntimeError("Setting MMG2D_IPARAM_verbose failed.");
    }
    if (MMG2D_Set_dparameter(mmgMesh, mmgSol, MMG2D_DPARAM_hmax, tol) != 1) {
        throw RuntimeError("Setting MMG2D_DPARAM_hmax failed.");
    }

    if (MMG2D_Set_meshSize(mmgMesh,num_input_vertices,num_input_faces,0) != 1) {
        throw RuntimeError("MMG2D_Set_meshSize failed.");
    }

    for (size_t i=0; i<num_input_vertices; i++) {
        if (MMG2D_Set_vertex(mmgMesh, m_vertices(i,0), m_vertices(i, 1), i+1, i+1) != 1) {
            throw RuntimeError("MMG2D_Set_vertex failed for vertex "
                    + std::to_string(i) + ".");
        }
    }

    for (size_t i=0; i<num_input_faces; i++) {
        if (MMG2D_Set_triangle(mmgMesh,
                    m_faces(i, 0) + 1,
                    m_faces(i, 1) + 1,
                    m_faces(i, 2) + 1,
                    i+1, i+1) != 1 )  {
            throw RuntimeError("MMG2D_Set_edge failed for edge "
                    + std::to_string(i) + ".");
        }
    }

    // Set per-vertex metric.  This must be done after mmgMesh is initialized.
    const size_t num_metrics = metric.rows();
    if (num_metrics == 0) {
        if (MMG2D_Set_solSize(mmgMesh, mmgSol, MMG5_Vertex, 0, MMG5_Scalar) != 1) {
            throw RuntimeError("MMG2D_Set_solSize failed.");
        }
    } else if (num_metrics == num_input_vertices) {
        if (MMG2D_Set_solSize(mmgMesh, mmgSol, MMG5_Vertex, num_metrics, MMG5_Scalar) != 1) {
            throw RuntimeError("MMG2D_Set_solSize failed.");
        }
        for (size_t i=0; i<num_metrics; i++) {
            if (MMG2D_Set_scalarSol(mmgSol, metric(i,0), i+1) != 1 ) {
                throw RuntimeError("MMG2D_Set_scalarSol failed.");
            }
        }
    }


    if (MMG2D_Chk_meshData(mmgMesh, mmgSol) != 1) {
        throw RuntimeError("MMG2D_Chk_meshData failed");
    }

    int r = MMG2D_mmg2dlib(mmgMesh,mmgSol);
    switch (r) {
        case MMG5_SUCCESS:
            break;
        case MMG5_LOWFAILURE:
            std::cerr << "Warning: MMG encountered low failure.  Results may not not be valid."
                << std::endl;
            break;
        case MMG5_STRONGFAILURE:
            throw RuntimeError("MMG strong failure!");
        default:
            throw NotImplementedError("Unknown MMG failure encountered.");
    }

    int num_vertices, num_triangles, num_edges;
    if (MMG2D_Get_meshSize(mmgMesh,
                &num_vertices, &num_triangles, &num_edges) !=1) {
        throw RuntimeError("MMG2D_Get_meshSize failed.");
    }

    m_vertices.resize(num_vertices, 2);
    int ref, is_corner, is_required;
    for (size_t i=0; i<num_vertices; i++) {
        if (MMG2D_Get_vertex(mmgMesh,
                    &m_vertices(i, 0),
                    &m_vertices(i, 1),
                    &ref,
                    &is_corner,
                    &is_required) != 1 ) {
            throw RuntimeError("MMG2D_Get_vertex failed.");
        }
    }

    m_faces.resize(num_triangles, 3);
    for (size_t i=0; i<num_triangles; i++) {
        if (MMG2D_Get_triangle(mmgMesh,
                    &m_faces(i, 0),
                    &m_faces(i, 1),
                    &m_faces(i, 2),
                    &ref,
                    &is_required) != 1 ) {
            throw RuntimeError("MMG2D_Get_triangle failed.");
        }
    }

    // MMG is 1 based unfortunately.
    m_faces.array() -= 1;

    //MMG2D_saveMesh(mmgMesh, "mmg_debug.mesh");

    MMG2D_Free_all(MMG5_ARG_start,
            MMG5_ARG_ppMesh,&mmgMesh,MMG5_ARG_ppMet,&mmgSol,
            MMG5_ARG_end);
}

#endif
