/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

#include <vector>
#include <Misc/TripletMap.h>

namespace PyMesh {
namespace EdgeUtils {
    /**
     * Form the longest edge chains possible from the input edges.
     * Assuming all vertex valance is either 1 or 2.
     */
    std::vector<VectorI> chain_edges(const MatrixIr& edges);

    TripletMap<size_t> compute_edge_face_adjacency(const MatrixIr& faces);
}
}
