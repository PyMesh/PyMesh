/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

class TetrahedralizationEngine {
    public:
        typedef std::shared_ptr<TetrahedralizationEngine> Ptr;
        static Ptr create(const std::string& engine_name);

    public:
        TetrahedralizationEngine() = default;
        virtual ~TetrahedralizationEngine() = default;

    public:
        virtual void run() {
            throw NotImplementedError(
                    "Tetrahedralization algorithm not implemented");
        }

        void set_vertices(const MatrixFr& vertices) { m_vertices = vertices; }
        void set_faces(const MatrixIr& faces) { m_faces = faces; }

        /**
         * max bound on ratio of the circumradius of a tet to its shortest edge.
         */
        void set_cell_radius_edge_ratio(Float val) {
            if (val <= 0.0) {
                throw RuntimeError("Cell radius edge ratio must be positive!");
            }
            if (val < 2.0) {
                std::cerr <<
                    "Warning: Setting radius to edge ratio below 2.0 "
                    "may cause the algorithm to not terminate!" << std::endl;
            }
            m_cell_radius_edge_ratio = val;
        }

        /**
         * cell size is the max radii of the circumsphere of tets.
         */
        void set_cell_size(Float val) {
            if (val < 0.0) {
                throw RuntimeError("Cell size must be positive!");
            }
            m_cell_size = val;
        }

        /**
         * Distance between surface facet and actual domain.
         */
        void set_facet_distance(Float val) {
            if (val < 0.0) {
                throw RuntimeError("Facet distance must be positive!");
            }
            m_facet_distance = val;
        }

        /**
         * Set angle threshold (in degrees) for feature extraction.
         */
        void set_feature_angle(Float val) {
            if (val < 0.0 || val > 180.0) {
                throw RuntimeError("Feature angle must be in the range [0, 180]!");
            }
            m_feature_angle = val;
        }

        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        MatrixIr get_voxels() const { return m_voxels; }

    protected:
        void preprocess();
        void assert_mesh_is_valid() const;
        void auto_compute_meshing_params();

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_voxels;

        Float m_ave_edge_length = 1.0;
        Float m_cell_radius_edge_ratio = 2.0;
        Float m_cell_size = -1.0;
        Float m_facet_distance = -1.0;
        Float m_feature_angle = 120.0; // degrees
};

}
