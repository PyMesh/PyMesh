/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE
#include <list>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class TriangleWrapper {
    public:
        TriangleWrapper() = default;

    public:
        using Region = std::list<size_t>;
        using Regions = std::list<Region>;
        enum class Algorithm {
            DIVIDE_AND_CONQUER, // Default algorithm.
            SWEEPLINE, // Steven Fortune's sweepline algorithm (-F option)
            INCREMENTAL   // Incremental algorithm (-i options).
        };

    public:
        /**
         * Add a point cloud to be triangulated/Voronoi diagrammed.
         * If the points are embedded in 3D space, they will be projected onto
         * a 2D plane based on PCA.
         */
        void set_points(const MatrixFr& points) { m_points = points; }
        const MatrixFr& get_points() const { return m_points; }

        /**
         * Add segment constraints.  It is equivalent to passing a PSLG as
         * input.
         */
        void set_segments(const MatrixIr& segments) { m_segments = segments; }
        const MatrixIr& get_segments() const { return m_segments; }

        /**
         * Add existing triangulation of the point cloud.  Used for refining an
         * existing triangulation.  This is equivalent to passing a .node
         * (specified with `set_points()`) and a .ele file as input (specified
         * with this functions).
         */
        void set_triangles(const MatrixIr& triangles) { m_triangles = triangles; }
        const MatrixIr& get_triangles() const { return m_triangles; }

        /**
         * Set hole points.  Used by triangle to flood and remove faces
         * representing holes.
         */
        void set_holes(const MatrixFr& holes) { m_holes = holes; }
        const MatrixFr& get_holes() const { return m_holes; }

        /**
         * Set min angle constraint (-q option, default 20 degrees).
         * Warning: min_angle > 20.7 degrees may cause algorithm to not
         * terminate.
         */
        void set_min_angle(const Float min_angle) { m_min_angle = min_angle; }
        const Float get_min_angle() const { return m_min_angle; }

        /**
         * Set max area constraint (-a option).
         * Negative value means this options is not used.
         */
        void set_max_area(const Float max_area) { m_max_area = max_area; }
        const Float get_max_area() const { return m_max_area; }

        void set_max_area_list(const VectorF& areas) { m_max_areas = areas; }
        const VectorF& get_max_area_list() const { return m_max_areas; }

        /**
         * Keep the entire covnex hull instead of eliminating triangles outside
         * of the PSLG (-c option, default is off).
         */
        void keep_convex_hull(bool convex_hull) { m_convex_hull = convex_hull; }
        bool get_keep_convex_hull() const { return m_convex_hull; }

        /**
         * Use __conforming__ Delaunay triangulation where all output triangles
         * are required to be Delaunay instead of constrained Delaunay.
         * (-D option, default is off).
         */
        void set_conforming_delaunay(bool conforming) { m_conforming = conforming; }
        bool get_conforming_delaunay() const { return m_conforming; }

        /**
         * Use exact arithmetic.  (-X option, default is to use exact arithmetic).
         */
        void set_exact_arithmetic(bool exact) { m_exact = exact; }
        bool get_exact_arithmetic() const { return m_exact; }

        /**
         * Whether to add Steiner points to split input line segments on the
         * boundary.  (-Y option, default is to use Steiner points when
         * necessary).
         */
        void set_split_boundary(bool split) { m_split_boundary = split; }
        bool get_split_boundary() const { return m_split_boundary; }

        /**
         * Max number of Steiner points to use.  (-S option, negative means to
         * unlimited number of Steiner points, which is also the default).
         */
        void set_max_num_steiner_points(int max_num_steiner) {
            m_max_num_steiner = max_num_steiner;
        }
        int get_max_num_steiner_points() const {
            return m_max_num_steiner;
        }

        /**
         * level=0 means completely quiet (-Q option).
         *      =1 means normal level of verbosity (default).
         *      =2 means verbose output (-V option).
         *      =3 means gives vertex-by-vertex details (-VV option).
         *      =4 means you are seriously debugging triangle (-VVVV option).
         */
        void set_verbosity(short level) { m_verbose_level = level; }
        short get_verbosity() const { return m_verbose_level; }

        /**
         * Set the algorithm to use.  Default is divide and conquer.
         */
        void set_algorithm(Algorithm algorithm) { m_algorithm = algorithm; }
        Algorithm get_algorithm() const { return m_algorithm; }

        /**
         * Instead of specifying hole points, infer holes from winding number of
         * the input PSLG.  This is not part of the triangle pipeline, default
         * is false.
         */
        void set_auto_hole_detection(bool auto_hole) {
            m_auto_hole_detection = auto_hole;
        }
        bool get_auto_hole_detection() const { return m_auto_hole_detection; }

        /**
         * Generate commandline options and run triangulate() method from
         * triangle.
         */
        void run();

        const MatrixFr& get_vertices() const { return m_vertices; }
        const MatrixIr& get_faces() const { return m_faces; }
        const MatrixFr& get_voronoi_vertices() const { return m_voronoi_vertices; }
        const MatrixIr& get_voronoi_edges() const { return m_voronoi_edges; }
        const VectorI& get_regions() const { return m_regions; }

    protected:
        std::string generate_command_line_options() const;

        void process_2D_input(const std::string& flags);

        void run_triangle(const std::string& flags);
        void poke_holes(Regions& regions);
        bool select_seed_point(const Region& region, VectorF& seed_p);
        void correct_orientation();

    private:
        // Input options
        Float m_min_angle = 20.0; // degrees.
        Float m_max_area = -1.0; // Not set.
        bool m_convex_hull = false; // Not keeping convex hull.
        bool m_conforming = false; // Not require conforming.
        bool m_exact = true; // Use exact arithmetic.
        bool m_split_boundary = true; // Allow splitting of boundary.
        int m_max_num_steiner = -1; // Unlimited.
        short m_verbose_level = 1; // Normal
        Algorithm m_algorithm = Algorithm::DIVIDE_AND_CONQUER;
        bool m_auto_hole_detection = false;
        VectorF m_max_areas;

        // Input data
        MatrixFr m_points;
        MatrixFr m_holes;
        MatrixIr m_segments;
        MatrixIr m_triangles;

        // Output triangulation data
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_face_neighbors;
        VectorI  m_edges;
        VectorI  m_edge_marks;
        VectorI  m_regions;

        // Output Voronoi data
        MatrixFr m_voronoi_vertices;
        MatrixIr m_voronoi_edges;
};

}

#endif
