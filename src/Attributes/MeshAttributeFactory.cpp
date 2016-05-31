/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshAttributeFactory.h"

#include "EdgeLengthAttribute.h"
#include "EdgeSquaredLengthAttribute.h"
#include "EdgeDihedralAngleAttribute.h"
#include "FaceAreaAttribute.h"
#include "FaceAspectRatioAttribute.h"
#include "FaceCentroidAttribute.h"
#include "FaceCircumCenterAttribute.h"
#include "FaceCircumRadiusAttribute.h"
#include "FaceEdgeRatioAttribute.h"
#include "FaceIncircleCenterAttribute.h"
#include "FaceIncircleRadiusAttribute.h"
#include "FaceIndexAttribute.h"
#include "FaceNormalAttribute.h"
#include "FaceVoronoiAreaAttribute.h"
#include "MeshAttribute.h"
#include "VertexAreaAttribute.h"
#include "VertexDihedralAngleAttribute.h"
#include "VertexGaussianCurvatureAttribute.h"
#include "VertexIndexAttribute.h"
#include "VertexLaplacianAttribute.h"
#include "VertexMeanCurvatureAttribute.h"
#include "VertexNormalAttribute.h"
#include "VertexValanceAttribute.h"
#include "VertexVolumeAttribute.h"
#include "VoxelCentroidAttribute.h"
#include "VoxelIndexAttribute.h"
#include "VoxelVolumeAttribute.h"

using namespace PyMesh;

MeshAttribute::Ptr PyMesh::MeshAttributeFactory::create(
        const std::string& name) {
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
    } else if (name == "vertex_gaussian_curvature") {
        MeshAttribute::Ptr attr(new VertexGaussianCurvatureAttribute(name));
        return attr;
    } else if (name == "vertex_index") {
        MeshAttribute::Ptr attr(new VertexIndexAttribute(name));
        return attr;
    } else if (name == "vertex_valance") {
        MeshAttribute::Ptr attr(new VertexValanceAttribute(name));
        return attr;
    } else if (name == "vertex_dihedral_angle") {
        MeshAttribute::Ptr attr(new VertexDihedralAngleAttribute(name));
        return attr;
    } else if (name == "edge_length") {
        MeshAttribute::Ptr attr(new EdgeLengthAttribute(name));
        return attr;
    } else if (name == "edge_squared_length") {
        MeshAttribute::Ptr attr(new EdgeSquaredLengthAttribute(name));
        return attr;
    } else if (name == "edge_dihedral_angle") {
        MeshAttribute::Ptr attr(new EdgeDihedralAngleAttribute(name));
        return attr;
    } else if (name == "face_area") {
        MeshAttribute::Ptr attr(new FaceAreaAttribute(name));
        return attr;
    } else if (name == "face_aspect_ratio") {
        MeshAttribute::Ptr attr(new FaceAspectRatioAttribute(name));
        return attr;
    } else if (name == "face_centroid") {
        MeshAttribute::Ptr attr(new FaceCentroidAttribute(name));
        return attr;
    } else if (name == "face_circumcenter") {
        MeshAttribute::Ptr attr(new FaceCircumCenterAttribute(name));
        return attr;
    } else if (name == "face_circumradius") {
        MeshAttribute::Ptr attr(new FaceCircumRadiusAttribute(name));
        return attr;
    } else if (name == "face_edge_ratio") {
        MeshAttribute::Ptr attr(new FaceEdgeRatioAttribute(name));
        return attr;
    } else if (name == "face_incircle_center") {
        MeshAttribute::Ptr attr(new FaceIncircleCenterAttribute(name));
        return attr;
    } else if (name == "face_incircle_radius") {
        MeshAttribute::Ptr attr(new FaceIncircleRadiusAttribute(name));
        return attr;
    } else if (name == "face_index") {
        MeshAttribute::Ptr attr(new FaceIndexAttribute(name));
        return attr;
    } else if (name == "face_normal") {
        MeshAttribute::Ptr attr(new FaceNormalAttribute(name));
        return attr;
    } else if (name == "face_voronoi_area") {
        MeshAttribute::Ptr attr(new FaceVoronoiAreaAttribute(name));
        return attr;
    } else if (name == "voxel_index") {
        MeshAttribute::Ptr attr(new VoxelIndexAttribute(name));
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
