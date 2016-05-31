/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

namespace PyMesh {

/**
 * This class removes duplicated triangle faces.
 * Two faces are duplicates of each other if they consist of the same 3
 * vertices.
 */
class FinFaceRemoval {
    public:
        FinFaceRemoval(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        /**
         * Only remove fins.  A fin is defined as a pair of faces with identical
         * vertices but opposite orientaiton.
         */
        void set_fins_only() {
            m_fins_only = true;
        }

        /**
         * When set_fins_only() is called:
         * Only fins are removed.
         *
         * Otherwise:
         *
         * For each set of duplicated faces, if there is a majority orientation,
         * all but a single face is removed.  The face kept has the same
         * orientation as the majority orientation.  This is necessary to remove
         * "folds".
         *
         * If there is no majority orientation, all faces in the set are
         * removed.  This is necessary in removing "fins".
         */
        size_t run();
        MatrixFr get_vertices() const { return m_vertices; }
        MatrixIr get_faces() const { return m_faces; }
        VectorI  get_face_indices() const { return m_face_indices; }

    private:
        bool     m_fins_only;
        MatrixFr m_vertices;
        MatrixIr m_faces;
        VectorI  m_face_indices;
};

}
