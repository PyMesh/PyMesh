/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "TilerEngine.h"
#include <Mesh.h>

namespace PyMesh {

class MeshTiler : public TilerEngine {
    public:
        typedef Mesh::Ptr MeshPtr;

        MeshTiler(WireNetwork::Ptr unit_wire_network, MeshPtr mesh);
        virtual ~MeshTiler() {}

    public:
        virtual WireNetwork::Ptr tile();

    private:
        WireNetwork::Ptr tile_2D();
        WireNetwork::Ptr tile_3D();

        void scale_to_unit_box();

        void evaluate_parameters(
                WireNetwork& wire_network, const FuncList& funcs);
        void evaluate_thickness_parameters(
                WireNetwork& wire_network, const FuncList& funcs);
        void evaluate_offset_parameters(
                WireNetwork& wire_network, const FuncList& funcs);

    private:
        MeshPtr m_mesh;
};

}
