/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <Core/EigenTypedef.h>

namespace PyMesh {

/**
 * FEMeshAdaptor is an adaptor interface to represent finite element mesh structure.
 */
class FEMeshAdaptor {
    public:
        typedef std::shared_ptr<FEMeshAdaptor> Ptr;

    public:
        virtual ~FEMeshAdaptor() {}

    public:
        virtual size_t getDim() const=0;

        // Node queries
        virtual size_t getNbrNodes() const=0;
        virtual VectorF getNodes() const=0;
        virtual const VectorF& getNodesRaw() const=0;
        virtual VectorF getNode(size_t vi) const=0;
        virtual VectorI getNodeAdjacentNodes(size_t vi) const=0;
        virtual VectorI getNodeAdjacentElements(size_t vi) const=0;
        virtual bool isBoundaryNode(size_t vi) const=0;

        // Element queries
        virtual size_t getNbrElements() const=0;
        virtual size_t getNodePerElement() const=0;
        virtual VectorI getElements() const=0;
        virtual const VectorI& getElementsRaw() const=0;
        virtual VectorI getElement(size_t ei) const=0;
        virtual VectorI getElementAdjacentElements(size_t ei) const=0;
        virtual Float   getElementVolume(size_t ei) const=0;
        virtual bool isBoundaryElement(size_t ei) const=0;

        // Boundary faces queries
        virtual size_t getNbrBoundaryFaces() const=0;
        virtual size_t getNodePerBoundaryFace() const=0;
        virtual VectorI getBoundaryFace(size_t bfi) const=0; // Return array of vi
        virtual Float   getBoundaryFaceArea(size_t bfi) const=0;
        virtual VectorF getBoundaryFaceNormal(size_t bfi) const=0;

        // Boundary nodes queries
        virtual size_t getNbrBoundaryNodes() const=0;
        virtual VectorI getBoundaryNodes() const=0;
        virtual const VectorI& getBoundaryNodesRaw() const=0;
        virtual size_t  getBoundaryNode(size_t bvi) const=0; // Return vi
        virtual size_t  getBoundaryIndex(size_t vi) const=0; // Return bvi
        virtual VectorF getBoundaryNodeNormal(size_t bvi) const=0;
        virtual VectorI getBoundaryNodeAdjacentBoundaryFaces(size_t bvi) const=0;
        virtual VectorI getBoundaryNodeAdjacentBoundaryNodes(size_t bvi) const=0;
};

}
