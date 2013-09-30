#pragma once

#include <vector>
#include <set>

#include "EigenTypedef.h"
#include "CGAL_includes.h"

class BoundingBoxes {
    public:
        BoundingBoxes(const MatrixF& vertices, const MatrixI& faces);

    public:
        void check_overlap();
        void clear();
        VectorI get_overlapping_obj_indices(size_t id) const;

    public:
        // Callback function.
        void mark_as_overlap(const TrianglesIterator& b1, const TrianglesIterator& b2) {
            size_t id1 = b1 - m_mesh.begin();
            size_t id2 = b2 - m_mesh.begin();
            m_overlap_array[id1].push_back(id2);
            m_overlap_array[id2].push_back(id1);
        }

    private:
        std::vector<Box> get_triangle_bboxes();

    private:
        typedef std::vector<size_t> IndexArray;
        typedef std::vector<std::vector<size_t> > IndexArraies;

        IndexArraies m_overlap_array;
        Triangles m_mesh;
};
