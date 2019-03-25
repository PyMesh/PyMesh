/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <memory>
#include <vector>
#include <Mesh.h>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class MeshCutter {
    public:
        using Ptr = std::shared_ptr<MeshCutter>;

    public:
        MeshCutter(Mesh::Ptr mesh);

        /**
         * Edge separating faces with different labels are cut.
         */
        Mesh::Ptr cut_with_face_labels(const std::vector<size_t>& comp_ids) const;

        /**
         * Edge that has discontinuous corner attribute on each side is cut.
         */
        Mesh::Ptr cut_at_uv_discontinuity() const;

        /**
         * Cut along edge chains.
         */
        Mesh::Ptr cut_along_edges(
                const std::vector<std::vector<int>>& edge_chains) const;

    private:
        Mesh::Ptr m_mesh;
};

}


