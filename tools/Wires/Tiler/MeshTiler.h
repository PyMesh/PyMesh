#pragma once

#include "TilerEngine.h"
#include <Mesh.h>

class MeshTiler : public TilerEngine {
    public:
        typedef Mesh::Ptr MeshPtr;

        MeshTiler(const WireNetwork& unit_wire_network, MeshPtr mesh);
        virtual ~MeshTiler() {}

    public:
        virtual WireNetwork tile();

    private:
        WireNetwork tile_2D();
        WireNetwork tile_3D();

        void scale_to_unit_box();

    private:
        MeshPtr m_mesh;
};
