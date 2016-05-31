/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Elements.h"

#include <sstream>

#include <Core/Exception.h>

#include "EdgeElements.h"
#include "TetrahedronElements.h"
#include "TriangleElements.h"

using namespace PyMesh;

Elements::Ptr Elements::adapt(Mesh::Ptr mesh) {
    if (mesh->get_num_voxels() > 0) {
        const size_t vertex_per_voxel = mesh->get_vertex_per_voxel();
        switch (vertex_per_voxel) {
            case 4:
                return Ptr(new TetrahedronElements(mesh));
            default:
                std::stringstream err_msg;
                err_msg << "Voxel with " << vertex_per_voxel
                    << " vertices is not supported yet.";
                throw NotImplementedError(err_msg.str());
        }
    } else {
        const size_t vertex_per_face = mesh->get_vertex_per_face();
        switch (vertex_per_face) {
            case 3:
                return Ptr(new TriangleElements(mesh));
            default:
                std::stringstream err_msg;
                err_msg << "Face with " << vertex_per_face
                    << " vertices is not supported yet.";
                throw NotImplementedError(err_msg.str());
        }
    }
}

Elements::Ptr Elements::adapt_boundary(Mesh::Ptr mesh) {
    if (mesh->get_num_voxels() > 0) {
        const size_t vertex_per_voxel = mesh->get_vertex_per_voxel();
        switch (vertex_per_voxel) {
            case 4:
                return Ptr(new TriangleElements(mesh));
            default:
                std::stringstream err_msg;
                err_msg << "Voxel with " << vertex_per_voxel
                    << " vertices is not supported yet.";
                throw NotImplementedError(err_msg.str());
        }
    } else {
        const size_t vertex_per_face = mesh->get_vertex_per_face();
        switch (vertex_per_face) {
            case 3:
                return Ptr(new EdgeElements(mesh));
            default:
                std::stringstream err_msg;
                err_msg << "Face with " << vertex_per_face
                    << " vertices is not supported yet.";
                throw NotImplementedError(err_msg.str());
        }
    }
}
