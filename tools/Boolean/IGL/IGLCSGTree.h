#pragma once

#include <Boolean/CSGTree.h>
#include <igl/copyleft/cgal/CSGTree.h>

namespace PyMesh {

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
        virtual VectorI get_mesh_sources() const;
        virtual MatrixFr get_vertices() const;
        virtual MatrixIr get_faces() const;
        virtual size_t get_num_vertices() const;
        virtual size_t get_num_faces() const;
        virtual std::vector<size_t> get_birth_face_sizes() const;

    protected:
        typedef igl::copyleft::cgal::CSGTree IGLTree;
        IGLTree m_igl_tree;
};

}
