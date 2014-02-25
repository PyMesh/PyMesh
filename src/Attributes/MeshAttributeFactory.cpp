#include "MeshAttributeFactory.h"

#include <tr1/memory>

#include "EdgeLengthAttribute.h"
#include "FaceAreaAttribute.h"
#include "FaceNormalAttribute.h"
#include "MeshAttribute.h"
#include "VertexAreaAttribute.h"
#include "VertexLaplacianAttribute.h"
#include "VertexMeanCurvatureAttribute.h"
#include "VertexNormalAttribute.h"
#include "VertexVolumeAttribute.h"
#include "VoxelCentroidAttribute.h"
#include "VoxelVolumeAttribute.h"

MeshAttribute::Ptr MeshAttributeFactory::create(const std::string& name) {
    if (name == "vertex_normal") {
        MeshAttribute::Ptr attr(new VertexNormalAttribute(name));
        return attr;
    } else if (name == "vertex_volume") {
        MeshAttribute::Ptr attr(new VertexVolumeAttribute(name));
        return attr;
    } else if (name == "vertex_area") {
        MeshAttribute::Ptr attr(new VertexAreaAttribute(name));
        return attr;
    } else if (name == "vertex_laplacian") {
        MeshAttribute::Ptr attr(new VertexLaplacianAttribute(name));
        return attr;
    } else if (name == "vertex_mean_curvature") {
        MeshAttribute::Ptr attr(new VertexMeanCurvatureAttribute(name));
        return attr;
    } else if (name == "edge_length") {
        MeshAttribute::Ptr attr(new EdgeLengthAttribute(name));
        return attr;
    } else if (name == "face_area") {
        MeshAttribute::Ptr attr(new FaceAreaAttribute(name));
        return attr;
    } else if (name == "face_normal") {
        MeshAttribute::Ptr attr(new FaceNormalAttribute(name));
        return attr;
    } else if (name == "voxel_volume") {
        MeshAttribute::Ptr attr(new VoxelVolumeAttribute(name));
        return attr;
    } else if (name == "voxel_centroid") {
        MeshAttribute::Ptr attr(new VoxelCentroidAttribute(name));
        return attr;
    } else {
        // Generic attribute.
        MeshAttribute::Ptr attr(new MeshAttribute(name));
        return attr;
    }
}
