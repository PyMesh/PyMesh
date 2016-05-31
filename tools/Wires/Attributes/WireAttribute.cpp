/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireAttribute.h"
#include "WireEdgeCubicSymmetryAttribute.h"
#include "WireEdgeLengthAttribute.h"
#include "WireEdgePeriodicIndexAttribute.h"
#include "WireEdgeSymmetryAttribute.h"
#include "WireVertexCubicSymmetryAttribute.h"
#include "WireVertexMinAngleAttribute.h"
#include "WireVertexPeriodicIndexAttribute.h"
#include "WireVertexSupportAttribute.h"
#include "WireVertexSymmetryAttribute.h"

#include <cassert>

using namespace PyMesh;

WireAttribute::Ptr WireAttribute::create(const std::string& name,
        WireAttribute::AttributeType type) {
    if (name == "vertex_min_angle") {
        return Ptr(new WireVertexMinAngleAttribute);
    } else if (name == "vertex_periodic_index") {
        return Ptr(new WireVertexPeriodicIndexAttribute);
    } else if (name == "vertex_symmetry_orbit") {
        return Ptr(new WireVertexSymmetryAttribute);
    } else if (name == "vertex_cubic_symmetry_orbit") {
        return Ptr(new WireVertexCubicSymmetryAttribute);
    } else if (name == "vertex_support_X") {
        Vector3F print_dir = Vector3F::UnitX();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "vertex_support_Y") {
        Vector3F print_dir = Vector3F::UnitY();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "vertex_support_Z") {
        Vector3F print_dir = Vector3F::UnitZ();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "vertex_support_XY") {
        Vector3F print_dir = Vector3F(1.0, 1.0, 0.0).normalized();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "vertex_support_YZ") {
        Vector3F print_dir = Vector3F(0.0, 1.0, 1.0).normalized();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "vertex_support_ZX") {
        Vector3F print_dir = Vector3F(1.0, 0.0, 1.0).normalized();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "vertex_support_XYZ") {
        Vector3F print_dir = Vector3F(1.0, 1.0, 1.0).normalized();
        return Ptr(new WireVertexSupportAttribute(print_dir));
    } else if (name == "edge_length") {
        return Ptr(new WireEdgeLengthAttribute);
    } else if (name == "edge_periodic_index") {
        return Ptr(new WireEdgePeriodicIndexAttribute);
    } else if (name == "edge_symmetry_orbit") {
        return Ptr(new WireEdgeSymmetryAttribute);
    } else if (name == "edge_cubic_symmetry_orbit") {
        return Ptr(new WireEdgeCubicSymmetryAttribute);
    } else {
        Ptr attr = Ptr(new WireAttribute);
        attr->set_attribute_type(type);
        return attr;
    }
}
