/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

#include <vector>

namespace PyMesh {
namespace FaceUtils {
    /**
     * Return true if all three points are exactly colinear.
     * I am using exact predicate from Shewchuk.
     * https://www.cs.cmu.edu/~quake/robust.html
     */
    bool is_colinear_2D(
            const Vector2F& v0,
            const Vector2F& v1,
            const Vector2F& v2);

    /**
     * Return true if all three points are exactly colinear.
     * Colinear test is done by checking the orientation of triangle projected
     * onto XY, YZ and ZX plane.  I am using exact predicate from Shewchuk.
     * https://www.cs.cmu.edu/~quake/robust.html
     */
    bool is_colinear_3D(
            const Vector3F& v0,
            const Vector3F& v1,
            const Vector3F& v2);


    /**
     * Return the list of face indices that is degenerated.
     */
    std::vector<size_t> get_degenerated_faces(
            const MatrixFr& vertices,
            const MatrixIr& faces);
}
}
