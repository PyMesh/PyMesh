/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <string>
#include <Core/EigenTypedef.h>

#include "GeometryCorrectionTable.h"

namespace PyMesh {

class WireProfile {
    public:
        typedef std::shared_ptr<WireProfile> Ptr;
        static Ptr create(const std::string& name);
        static Ptr create_isotropic(size_t num_samples);
        static Ptr create_2D();

        virtual ~WireProfile() {}

    public:
        /**
         * Loop is a sequence of vertices around the Z axis in 3D or Y axis
         * in 2D.
         */
        void initialize(const MatrixFr& loop);

        /**
         * apply geometric correction based on lookup table
         */
        void with_geometry_correction_lookup(const std::string& csv_file) {
            m_correction_table = std::make_shared<GeometryCorrectionTable>(csv_file);
        }

        /**
         * Place the edge loop along the edge (end_1, end_2) that is offseted
         * from end_1.  Thickness parameter specifies the diameter of the
         * bounding sphere of the loop.
         */
        MatrixFr place(const VectorF& end_1, const VectorF& end_2,
                Float offset, Float thickness,
                const VectorF& rel_correction,
                const VectorF& abs_correction,
                Float correction_cap,
                Float spread_const);

        size_t size() const { return m_loop.rows(); }

    protected:
        size_t m_dim;
        MatrixFr m_loop;
        Float m_radius;
        VectorF m_offset_dir;
        GeometryCorrectionTable::Ptr m_correction_table;
};

}
