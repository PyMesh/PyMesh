/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <Core/EigenTypedef.h>
#include <Wires/WireNetwork/WireNetwork.h>

namespace PyMesh {

class IsotropicDofExtractor {
    public:
        IsotropicDofExtractor(WireNetwork::Ptr wire_network);

    public:
        //std::vector<VectorF> extract_dofs(const VectorI& orbit);
        std::vector<VectorF> extract_dofs(const VectorF& v);

    private:
        void check_bbox_is_isotropic() const;
        //void check_orbit_cardinality(size_t orbit_size) const;
        std::vector<VectorF> extract_3D_dofs(const VectorF& v);
        std::vector<VectorF> extract_2D_dofs(const VectorF& v);

    private:
        WireNetwork::Ptr m_wire_network;
        VectorF m_bbox_min;
        VectorF m_bbox_max;
        VectorF m_bbox_center;
        Float m_bbox_half_size;
};

}
