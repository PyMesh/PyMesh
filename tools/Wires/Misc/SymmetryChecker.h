/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Misc/HashGrid.h>
#include <Wires/WireNetwork/WireNetwork.h>

namespace PyMesh {

class SymmetryChecker {
    public:
        SymmetryChecker(WireNetwork::Ptr wire_network);

        bool has_cubic_symmetry() const;

        /**
         * If wire_network has been changed, call update() to re-initiailize the
         * internal data structure.
         */
        void update();

    private:
        WireNetwork::Ptr m_wire_network;
        HashGrid::Ptr m_grid;
};

}
