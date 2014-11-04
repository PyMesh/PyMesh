#pragma once

#include "PeriodicInflator.h"

#include <list>
#include <vector>

class PeriodicInflator3D : public PeriodicInflator {
    public:
        PeriodicInflator3D(WireNetwork& wire_network) :
            PeriodicInflator(wire_network) {}
        virtual ~PeriodicInflator3D() {}

    protected:
        virtual void clip_to_center_cell();
        void clip_phantom_mesh();
        void update_face_sources();

        void enforce_periodicity();

        typedef std::vector<short> BDLabels;
        typedef std::list<Float> VertexList;
        typedef std::list<size_t> FaceList;

        void label_boundary_faces(size_t axis,
                BDLabels& boundary_face_labels);

        void extract_interior_faces(
                const BDLabels& bd_labels,
                VertexList& v_list, FaceList& f_list);
        void retriangulate_boundaries(
                const BDLabels& bd_labels,
                VertexList& v_list, FaceList& f_list);

        void extract_boundary_faces(
                const BDLabels& bd_labels, int label,
                MatrixFr& vertices, MatrixIr& faces);

    private:
        VectorF m_center_cell_bbox_min;
        VectorF m_center_cell_bbox_max;
};
