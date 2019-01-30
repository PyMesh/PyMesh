/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_TETGEN
#include <string>
#include <tetgen.h>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class Mesh;

class TetgenWrapper {
    public:
        /**
         * Set input points.
         */
        void set_points(const MatrixFr& points) { m_points = points; }
        const MatrixFr& get_points() const { return m_points; }

        /**
         * Set input triangle.
         */
        void set_triangles(const MatrixIr& triangles) { m_triangles = triangles; }
        const MatrixIr& get_triangles() const { return m_triangles; }

        /**
         * Set input tets (optional).  Used for refinement and coarsening.
         */
        void set_tetrahedra(const MatrixIr& tetrahedra) { m_tets = tetrahedra; }
        const MatrixIr& get_tetrahedra() const { return m_tets; }

        /**
         * User defined per-point integer marker (optional).  Typically
         *   0: vertex is interior.
         *   1: vertex is on boundary.
         *  -1: vertex should be removed.
         * But it could be anything.
         */
        void set_point_markers(const VectorI& point_markers) {
            m_point_markers = point_markers;
        }
        const VectorI& get_point_markers() const {
            return m_point_markers;
        }

        /**
         * Set point weights for weighted delaunay tetrahedralization.
         */
        void set_point_weights(const VectorF& point_weights) {
            m_point_weights = point_weights;
        }
        const VectorF& get_point_weights() const {
            return m_point_weights;
        }

        /**
         * User defined per-triangle integer marker (optional).  Typically
         *   0: triangle is interior.
         *   1: triangle is on boundary.
         * But it could be anything.
         */
        void set_triangle_markers(const VectorI& triangle_markers) {
            m_triangle_markers = triangle_markers;
        }
        const VectorI& get_triangle_markers() const {
            return m_triangle_markers;
        }

        /**
         * Allow boundary triangles to be split. (-Y option, default is true.)
         */
        void set_split_boundary(bool split) { m_split = split; }
        bool get_split_boundary() const { return m_split; }

        /**
         * Set max radius to edge ratio constraint. (-q option, first half,
         * default is 2.0)
         */
        void set_max_radius_edge_ratio(Float max_radius_edge_ratio) {
            m_max_radius_edge_ratio = max_radius_edge_ratio;
        }
        Float get_max_radius_edge_ratio() const {
            return m_max_radius_edge_ratio;
        }

        /**
         * Set min dihedral angle constraint. (-q option, second half,
         * default is 0.0)
         */
        void set_min_dihedral_angle(Float min_dihedral) {
            m_min_dihedral = min_dihedral;
        }
        Float get_min_dihedral_angle() const {
            return m_min_dihedral;
        }

        /**
         * Set whether to coarsen the input tet mesh.  (-R option, default is
         * false.)
         */
        void set_coarsening(bool coarsening) { m_coarsening = coarsening; }
        bool get_coarsening() const { return m_coarsening; }

        /**
         * Set max tet volume constraint.  (-a option, default is unlimited.)
         */
        void set_max_tet_volume(Float max_volume) { m_max_tet_volume = max_volume; }
        Float get_max_tet_volume() const { return m_max_tet_volume; }

        /**
         * Set mesh optmization level.  Valid value ranges from 0 t 10.
         * (-O option, default is 2.)
         */
        void set_optimization_level(short level) { m_optimization_level = level; }
        short get_optimization_level() const { return m_optimization_level; }

        /**
         * Set max number of Steiner points.  (-S option, default is unlimited.)
         */
        void set_max_num_steiner_points(int max_num_steiner) {
            m_max_num_steiner = max_num_steiner;
        }
        int get_max_num_steiner_points() const {
            return m_max_num_steiner;
        }

        /**
         * Set coplanar tolerance.  A tet with v/l^3 < tol are considered to
         * have coplanar vertices, where v and l are volume and average edge
         * length.  (-T option, default is 1e-8).
         */
        void set_coplanar_tolerance(Float tol) { m_coplanar_tol = tol; }
        Float get_coplanar_tolerance() const { return m_coplanar_tol; }

        /**
         * Use exact arithmetics. (-X option, default is true.)
         */
        void set_exact_arithmetic(bool exact) { m_exact = exact; }
        bool get_exact_arithmetic() const { return m_exact; }

        /**
         * Whether to merge coplanar facets or close vertices. (-M option,
         * default to false.)
         */
        void set_merge_coplanar(bool val) { m_merge_coplanar = val; }
        bool get_merge_coplanar() const { return m_merge_coplanar; }

        /**
         * Whether to compute weighted delaunay tetraehdralization.  This option
         * requires point weights.  (-w option, * default is false.)
         */
        void set_weighted_delaunay(bool weighted) { m_weighted_delaunay = weighted; }
        bool get_weighted_delaunay() const { return m_weighted_delaunay; }

        /**
         * Whether to keep all tets in the convex hull.  (-c option, default is
         * false.)
         */
        void set_keep_convex_hull(bool keep) { m_keep_convex_hull = keep; }
        bool get_keep_convex_hull() const { return m_keep_convex_hull; }

        /**
         * Set verbosity level.
         * Level =0: Quiet (-Q option)
         *       =1: Normal level (default)
         *       =2: Verbose output, including quality report (-V option.)
         *       =3: More details about the algorithm. (-VV option.)
         *       =4: You must be debugging tetgen.  (-VVV option.)
         */
        void set_verbosity(short verbosity) { m_verbosity = verbosity; }
        short get_verbosity() const { return m_verbosity; }

        void run();

        const MatrixFr& get_vertices() const { return m_tet_vertices; }
        const MatrixIr& get_faces() const { return m_tet_faces; }
        const MatrixIr& get_voxels() const { return m_tet_voxels; }
        const VectorI& get_regions() const { return m_regions; }

        std::string generate_command_line_options() const;

    private:
        // Tetgen inputs
        MatrixFr m_points;
        MatrixIr m_triangles;
        MatrixIr m_tets;
        VectorI m_point_markers;
        VectorF m_point_weights;
        VectorI m_triangle_markers;

        // Tetgen settings
        bool m_split = true;
        Float m_max_radius_edge_ratio = 2.0;
        Float m_min_dihedral = 0.0;
        bool m_coarsening = false;
        Float m_max_tet_volume = -1.0;
        int m_optimization_level = 2;
        int m_max_num_steiner = -1;
        Float m_coplanar_tol = 1e-8;
        bool m_exact = true;
        bool m_merge_coplanar = true;
        bool m_weighted_delaunay = false;
        bool m_keep_convex_hull = false;
        short m_verbosity = 1;

        // Tetgen outputs
        MatrixFr m_tet_vertices;
        MatrixIr m_tet_faces;
        MatrixIr m_tet_voxels;
        VectorI  m_regions;
};

}

#endif
