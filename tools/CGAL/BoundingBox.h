#pragma once

#include <vector>
#include <set>

#include <Core/EigenTypedef.h>

#include "CGAL_includes.h"

class BoundingBoxes {
    public:
        BoundingBoxes(const MatrixF& vertices, const MatrixI& faces);

    public:
        /**
         * Find all face pairs that "maybe" touching.
         * "Touching" means the distance between them is < threshold.
         * "Maybe" indicates false positive is possible.  Not all faces found
         * are actually touching, but all touching faces should be found.
         */
        void check_touching_faces(Float threshold, bool approximate=false);

        /**
         * Clear previous computed results.
         */
        void clear();

        /**
         * Return the indices of faces that could be "touching".
         */
        VectorI get_touching_face_indices(size_t id) const;

    public:
        // Callback function.
        void mark_as_touching(const TrianglesIterator& b1, const TrianglesIterator& b2) {
            size_t id1 = b1 - m_mesh.begin();
            size_t id2 = b2 - m_mesh.begin();
            m_touching_faces[id1].push_back(id2);
            m_touching_faces[id2].push_back(id1);
        }

        void review_touching_triangle_pairs(
                const TrianglesIterator& b1, const TrianglesIterator& b2);

    private:
        /**
         * Note bboxes are expanded by threshold.
         */
        std::vector<Box> get_triangle_bboxes(Float threshold);

    private:
        typedef std::vector<size_t> IndexArray;
        typedef std::vector<std::vector<size_t> > IndexArraies;

        IndexArraies m_touching_faces;
        Triangles m_mesh;

        Float m_threshold;
};
