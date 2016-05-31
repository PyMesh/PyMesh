/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CSGTree.h"
#include <sstream>
#include <Core/Exception.h>

#ifdef WITH_IGL
#include "IGL/IGLCSGTree.h"
#endif 

using namespace PyMesh;

CSGTree::Ptr CSGTree::create(const std::string& engine_name) {
#ifdef WITH_IGL
    if (engine_name == "igl") {
        return Ptr(new IGLCSGTree());
    }
#endif
    std::stringstream err_msg;
    err_msg << "CSG engine " << engine_name << " is not supported";
    throw NotImplementedError(err_msg.str());
}

CSGTree::Ptr CSGTree::create_leaf(const std::string& engine_name,
        const MatrixFr& vertices,
        const MatrixIr& faces) {
#ifdef WITH_IGL
    if (engine_name == "igl") {
        return Ptr(new IGLCSGTree(vertices, faces));
    }
#endif
    std::stringstream err_msg;
    err_msg << "CSG engine " << engine_name << " is not supported";
    throw NotImplementedError(err_msg.str());
}
