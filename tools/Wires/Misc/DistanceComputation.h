/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

namespace DistanceComputation {
    Float point_to_edge(
            const Vector3F& p, const Vector3F& e0, const Vector3F& e1);
    Float point_to_triangle(
            const Vector3F& p, const Vector3F& t0, const Vector3F& t2, const Vector3F& t3);
}

}
