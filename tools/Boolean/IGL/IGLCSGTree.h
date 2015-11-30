#pragma once

#include <Boolean/CSGTree.h>
#include <igl/copyleft/boolean/CSGTree.h>

class IGLCSGTree : public CSGTree {
    public:
        IGLCSGTree() {}
        IGLCSGTree(const MatrixFr& vertices, const MatrixIr& faces) :
            CSGTree(vertices, faces), m_igl_tree(vertices, faces) { }
        virtual ~IGLCSGTree() {}

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();
        virtual VectorI get_face_sources() const;
        virtual MatrixFr get_vertices() const;
        virtual MatrixIr get_faces() const;

    protected:
        typedef igl::copyleft::boolean::CSGTree IGLTree;
        IGLTree m_igl_tree;
};
