/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <MeshUtils/Boundary.h>
#include "FEMeshAdaptor.h"

namespace PyMesh {

class TriangleMesh : public FEMeshAdaptor {
    public:
        typedef Mesh::Ptr MeshPtr;
        TriangleMesh (MeshPtr mesh);
        virtual ~TriangleMesh() {}

    public:
        virtual size_t getDim() const;

        // Node queries
        virtual size_t getNbrNodes() const;
        virtual VectorF getNodes() const;
        virtual const VectorF& getNodesRaw() const;
        virtual VectorF getNode(size_t vi) const;
        virtual VectorI getNodeAdjacentNodes(size_t vi) const;
        virtual VectorI getNodeAdjacentElements(size_t vi) const;
        virtual bool isBoundaryNode(size_t vi) const;

        // Element queries
        virtual size_t getNbrElements() const;
        virtual size_t getNodePerElement() const;
        virtual VectorI getElements() const;
        virtual const VectorI& getElementsRaw() const;
        virtual VectorI getElement(size_t ei) const;
        virtual VectorI getElementAdjacentElements(size_t ei) const;
        virtual Float   getElementVolume(size_t ei) const;
        virtual bool isBoundaryElement(size_t ei) const;

        // Boundary faces queries
        virtual size_t getNbrBoundaryFaces() const;
        virtual size_t getNodePerBoundaryFace() const;
        virtual VectorI getBoundaryFace(size_t bfi) const; // Return array of vi
        virtual Float   getBoundaryFaceArea(size_t bfi) const;
        virtual VectorF getBoundaryFaceNormal(size_t bfi) const;

        // Boundary nodes queries
        virtual size_t getNbrBoundaryNodes() const;
        virtual VectorI getBoundaryNodes() const;
        virtual const VectorI& getBoundaryNodesRaw() const;
        virtual size_t  getBoundaryNode(size_t bvi) const; // Return vi
        virtual size_t  getBoundaryIndex(size_t vi) const; // Return bvi
        virtual VectorF getBoundaryNodeNormal(size_t bvi) const;
        virtual VectorI getBoundaryNodeAdjacentBoundaryFaces(size_t bvi) const;
        virtual VectorI getBoundaryNodeAdjacentBoundaryNodes(size_t bvi) const;

    private:
        typedef Boundary::Ptr BoundaryPtr;
        void verify_triangle_mesh();
        void init_boundary();
        void init_boundary_nodes(const BoundaryPtr& bd);
        void init_boundary_edges(const BoundaryPtr& bd);
        void init_boundary_node_adjacencies();
        void init_boundary_face_adjacencies();
        void init_boundary_lengths();
        void init_boundary_normals(const BoundaryPtr& bd);
        void init_boundary_vertex_normals();

    private:
        MeshPtr m_mesh;

        // Vector of size m (num boundary vertices), bvi -> vi
        VectorI m_boundary;
        // Vector of size n (num vertices)
        // vi -> bvi if vi lies on boundary
        // vi -> -1  if vi is not boundary
        VectorI m_boundary_idx;

        MatrixIr m_boundary_edges;

        // Boundary node adjacent boundary nodes
        VectorI m_boundary_node_adj;
        VectorI m_boundary_node_adj_idx;

        // Boundary node adjacent boundary edges
        VectorI m_boundary_node_edge_adj;
        VectorI m_boundary_node_edge_adj_idx;

        // Boundary attributes
        VectorF  m_boundary_lengths;
        MatrixFr m_boundary_normals;
        MatrixFr m_boundary_vertex_normals;
};

}
