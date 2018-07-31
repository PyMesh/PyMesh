/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#ifdef WITH_TETGEN
#include "TetgenWrapper.h"
#include <iostream>
#include <tetgen.h>

#include <Core/Exception.h>
#include <Mesh.h>

#include <tetgen.h>
#include "TetgenException.h"

using namespace PyMesh;

void TetgenWrapper::run() {
    tetgenio in, out, addin, bgmin;

    const size_t num_points = m_points.rows();
    const size_t num_triangles = m_triangles.rows();
    const size_t num_tets = m_tets.rows();
    const size_t num_point_markers = m_point_markers.size();
    const size_t num_point_weights = m_point_weights.size();
    const size_t num_triangle_markers = m_triangle_markers.size();

    in.firstnumber = 0;
    in.mesh_dim = 3;

    in.numberofpoints = m_points.rows();
    in.pointlist = new REAL[in.numberofpoints * 3];
    std::copy(m_points.data(), m_points.data() + m_points.size(),
            in.pointlist);

    if (num_point_markers > 0) {
        if (num_point_markers != num_points) {
            throw RuntimeError(
                    "Size of point marker must equal to the number of points");
        }
        in.pointmarkerlist = new int[num_points];
        std::copy(m_point_markers.data(), m_point_markers.data() + num_points,
                in.pointmarkerlist);
    }

    if (num_point_weights > 0) {
        if (num_point_weights != num_points) {
            throw RuntimeError(
                    "Size of point weights must equal to the nubmer of points");
        }
        in.numberofpointattributes = num_points;
        in.pointattributelist = new REAL[num_points];
        std::copy(m_point_weights.data(), m_point_weights.data() + num_points,
                in.pointattributelist);
    }

    if (num_triangles > 0) {
        if (m_triangles.cols() != 3) {
            throw NotImplementedError(
                    "Only triangles are supported as polygon input to tetgen");
        }
        in.numberoffacets = num_triangles;
        in.facetlist = new tetgenio::facet[num_triangles];
        for (size_t i=0; i<num_triangles; i++) {
            auto& f = in.facetlist[i];
            f.numberofpolygons = 1;
            f.polygonlist = new tetgenio::polygon[1];
            f.numberofholes = 0;
            f.holelist = NULL;
            auto& poly = f.polygonlist[0];
            poly.numberofvertices = 3;
            poly.vertexlist = new int[3];
            poly.vertexlist[0] = m_triangles(i, 0);
            poly.vertexlist[1] = m_triangles(i, 1);
            poly.vertexlist[2] = m_triangles(i, 2);
        }
    }
    if (num_triangle_markers > 0) {
        if (num_triangle_markers != num_triangles) {
            throw IOError(
                    "Number of triangle marker does not match number of triangles");
        }
        in.facetmarkerlist = new int[num_triangles];
        std::copy(m_triangle_markers.data(),
                m_triangle_markers.data() + num_triangles,
                in.facetmarkerlist);
    }

    if (num_tets > 0) {
        if (m_tets.cols() != 4) {
            throw IOError("Input tet must has 4 corners");
        }
        in.numberoftetrahedra = num_tets;
        in.tetrahedronlist = new int[num_tets * 4];
        std::copy(m_tets.data(), m_tets.data() + num_tets * 4, in.tetrahedronlist);
    }

    std::string flags = generate_command_line_options();
    try {
        tetrahedralize(const_cast<char*>(flags.c_str()), &in, &out);
    } catch (int error_code) {
        throw TetgenException(error_code);
    }

    m_tet_vertices.resize(out.numberofpoints, 3);
    std::copy(out.pointlist, out.pointlist + out.numberofpoints*3,
            m_tet_vertices.data());
    m_tet_faces.resize(out.numberoftrifaces, 3);
    std::copy(out.trifacelist, out.trifacelist + out.numberoftrifaces*3,
            m_tet_faces.data());
    m_tet_voxels.resize(out.numberoftetrahedra, 4);
    std::copy(out.tetrahedronlist, out.tetrahedronlist + out.numberoftetrahedra*4,
            m_tet_voxels.data());
}

std::string TetgenWrapper::generate_command_line_options() const {
    std::stringstream opt;
    opt.precision(16);
    opt << 'z';
    opt << "T" << m_coplanar_tol;
    bool is_PLC = false;
    if (m_coarsening) {
        opt << "R";
        is_PLC = true;
    } else if (m_tets.rows() > 0) {
        opt << "r";
        is_PLC = true;
    } else if (m_triangles.rows() > 0) {
        opt << "p";
        is_PLC = true;
    }
    if (is_PLC) {
        opt << "q" << m_max_radius_edge_ratio
            << "/" << m_min_dihedral;
        opt << "O" << m_optimization_level;
    }
    if (is_PLC && !m_split) {
        opt << "Y";
    }
    if (m_max_tet_volume > 0.0) {
        opt << "a" << m_max_tet_volume;
    }
    if (m_max_num_steiner >= 0) {
        opt << "S" << m_max_num_steiner;
    }
    if (!m_exact) {
        opt << "X";
    }
    if (!m_merge_coplanar) {
        opt << "M";
    }
    if (m_weighted_delaunay) {
        opt << "w";
    }
    if (m_keep_convex_hull) {
        opt << "c";
    }
    switch (m_verbosity) {
        case 0:
            opt << "Q";
            break;
        case 1:
            break;
        case 2:
            opt << "V";
            break;
        case 3:
            opt << "VV";
            break;
        case 4:
            opt << "VVV";
            break;
        default:
            throw RuntimeError("Unknown verbosity level: "
                    + std::to_string(m_verbosity));
    }
    return opt.str();
}

#endif
