#pragma once

#include "TilerEngine.h"
#include <Mesh.h>

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

    private:
        MeshPtr m_mesh;
};
