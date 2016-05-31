/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class TetrahedronizationEngine {
    public:
        typedef std::shared_ptr<TetrahedronizationEngine> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        TetrahedronizationEngine() :
            m_edge_size(-1.0),
            m_face_size(-1.0),
            m_cell_radius_edge_ratio(2),
            m_cell_size(-1.0),
            m_ave_edge_length(1.0) { }

        virtual ~TetrahedronizationEngine() {}

    public:
        virtual void run()=0;

        void set_vertices(const MatrixFr& vertices) { m_vertices = vertices; }
        void set_faces(const MatrixIr& faces) { m_faces = faces; }

        /**
         * edge size determines the sample density of the feature curves if any.
         */
        void set_edge_size(Float val) { m_edge_size = val; }

        /**
         * face size is the max radii of the circumcircle of surface triangles.
         */
        void set_face_size(Float val) { m_face_size = val; }

        /**
         * max bound on ratio of the circumradius of a tet to its shortest edge.
         */
        void set_cell_radius_edge_ratio(Float val) { m_cell_radius_edge_ratio = val; }

        /**
         * cell size is the max radii of the circumsphere of tets.
         */
        void set_cell_size(Float val) { m_cell_size = val; }

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        MatrixIr get_voxels() const { return m_voxels; }

    protected:
        void preprocess();
        void assert_mesh_is_valid() const;
        void compute_ave_edge_length();
        void auto_compute_meshing_params();

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_voxels;

        Float m_ave_edge_length;
        Float m_edge_size;
        Float m_face_size;
        Float m_cell_radius_edge_ratio;
        Float m_cell_size;
};

}
