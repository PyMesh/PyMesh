/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

namespace PyMesh {

/**
 * This function forces compiler to spell out what exact is the type of an
 * argument during linking.  Very handy for debugging crazily templated
 * libraries such as cgal and eigen...
 */
template <typename T> void type_check(const T& param);

}
