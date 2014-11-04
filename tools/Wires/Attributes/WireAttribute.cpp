#include "WireAttribute.h"
#include "WireEdgeLengthAttribute.h"
#include "WireEdgePeriodicIndexAttribute.h"
#include "WireVertexMinAngleAttribute.h"
#include "WireVertexPeriodicIndexAttribute.h"

#include <cassert>

WireAttribute::Ptr WireAttribute::create(const std::string& name,
        WireAttribute::AttributeType type) {
    if (name == "vertex_min_angle") {
        return Ptr(new WireVertexMinAngleAttribute);
    } else if (name == "vertex_periodic_index") {
        return Ptr(new WireVertexPeriodicIndexAttribute);
    } else if (name == "edge_length") {
        return Ptr(new WireEdgeLengthAttribute);
    } else if (name == "edge_periodic_index") {
        return Ptr(new WireEdgePeriodicIndexAttribute);
    } else {
        Ptr attr = Ptr(new WireAttribute);
        attr->set_attribute_type(type);
        return attr;
    }
}
