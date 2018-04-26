/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Core/EigenTypedef.h>
#include <Math/MatrixUtils.h>
#include "EdgeUtils.h"

namespace PyMesh {

VectorI detect_nonmanifold_vertices(const MatrixIr& faces);

}
