/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Mesh.h>

#include "Elements.h"

namespace PyMesh {

class TriangleElements : public Elements {
    public:
        TriangleElements(Mesh::Ptr mesh);

    public:
        virtual size_t getDim() const;

        virtual size_t getNbrNodes() const;
        virtual VectorF getNodes() const;
        virtual VectorF getNode(size_t vi) const;

        virtual size_t getNbrElements() const;
        virtual size_t getNodePerElement() const;
        virtual VectorI getElements() const;
        virtual VectorI getElement(size_t ei) const;
        virtual Float getElementVolume(size_t ei) const;
        virtual VectorF getElementCenter(size_t ei) const;

    private:
        void check_mesh();

    private:
        Mesh::Ptr m_mesh;
};

}
