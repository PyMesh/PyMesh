/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
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
#include "FaceFrameAttribute.h"
#include "FaceIncircleCenterAttribute.h"
#include "FaceIncircleRadiusAttribute.h"
#include "FaceIndexAttribute.h"
#include "FaceNormalAttribute.h"
#include "FaceRadiusEdgeRatioAttribute.h"
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
#include "VertexVoronoiAreaAttribute.h"
#include "VoxelCentroidAttribute.h"
#include "VoxelCircumCenterAttribute.h"
#include "VoxelCircumRadiusAttribute.h"
#include "VoxelDihedralAngleAttribute.h"
#include "VoxelEdgeRatioAttribute.h"
#include "VoxelFaceIndexAttribute.h"
#include "VoxelIncenterAttribute.h"
#include "VoxelInradiusAttribute.h"
#include "VoxelIndexAttribute.h"
#include "VoxelRadiusEdgeRatioAttribute.h"
#include "VoxelVolumeAttribute.h"

using namespace PyMesh;

MeshAttribute::Ptr PyMesh::MeshAttributeFactory::create(
        const std::string& name) {
    if (name == "vertex_normal") {
        return std::make_shared<VertexNormalAttribute>();
    } else if (name == "vertex_volume") {
        return std::make_shared<VertexVolumeAttribute>();
    } else if (name == "vertex_area") {
        return std::make_shared<VertexAreaAttribute>();
    } else if (name == "vertex_laplacian") {
        return std::make_shared<VertexLaplacianAttribute>();
    } else if (name == "vertex_mean_curvature") {
        return std::make_shared<VertexMeanCurvatureAttribute>();
    } else if (name == "vertex_gaussian_curvature") {
        return std::make_shared<VertexGaussianCurvatureAttribute>();
    } else if (name == "vertex_index") {
        return std::make_shared<VertexIndexAttribute>();
    } else if (name == "vertex_valance") {
        return std::make_shared<VertexValanceAttribute>();
    } else if (name == "vertex_dihedral_angle") {
        return std::make_shared<VertexDihedralAngleAttribute>();
    } else if (name == "vertex_voronoi_area") {
        return std::make_shared<VertexVoronoiAreaAttribute>();
    } else if (name == "edge_length") {
        return std::make_shared<EdgeLengthAttribute>();
    } else if (name == "edge_squared_length") {
        return std::make_shared<EdgeSquaredLengthAttribute>();
    } else if (name == "edge_dihedral_angle") {
        return std::make_shared<EdgeDihedralAngleAttribute>();
    } else if (name == "face_area") {
        return std::make_shared<FaceAreaAttribute>();
    } else if (name == "face_aspect_ratio") {
        return std::make_shared<FaceAspectRatioAttribute>();
    } else if (name == "face_centroid") {
        return std::make_shared<FaceCentroidAttribute>();
    } else if (name == "face_circumcenter") {
        return std::make_shared<FaceCircumCenterAttribute>();
    } else if (name == "face_circumradius") {
        return std::make_shared<FaceCircumRadiusAttribute>();
    } else if (name == "face_edge_ratio") {
        return std::make_shared<FaceEdgeRatioAttribute>();
    } else if (name == "face_frame") {
        return std::make_shared<FaceFrameAttribute>();
    } else if (name == "face_incircle_center") {
        return std::make_shared<FaceIncircleCenterAttribute>();
    } else if (name == "face_incircle_radius") {
        return std::make_shared<FaceIncircleRadiusAttribute>();
    } else if (name == "face_index") {
        return std::make_shared<FaceIndexAttribute>();
    } else if (name == "face_normal") {
        return std::make_shared<FaceNormalAttribute>();
    } else if (name == "face_radius_edge_ratio") {
        return std::make_shared<FaceRadiusEdgeRatioAttribute>();
    } else if (name == "face_voronoi_area") {
        return std::make_shared<FaceVoronoiAreaAttribute>();
    } else if (name == "voxel_dihedral_angle") {
        return std::make_shared<VoxelDihedralAngleAttribute>();
    } else if (name == "voxel_edge_ratio") {
        return std::make_shared<VoxelEdgeRatioAttribute>();
    } else if (name == "voxel_face_index") {
        return std::make_shared<VoxelFaceIndexAttribute>();
    } else if (name == "voxel_centroid") {
        return std::make_shared<VoxelCentroidAttribute>();
    } else if (name == "voxel_circumcenter") {
        return std::make_shared<VoxelCircumCenterAttribute>();
    } else if (name == "voxel_circumradius") {
        return std::make_shared<VoxelCircumRadiusAttribute>();
    } else if (name == "voxel_incenter") {
        return std::make_shared<VoxelIncenterAttribute>();
    } else if (name == "voxel_inradius") {
        return std::make_shared<VoxelInradiusAttribute>();
    } else if (name == "voxel_index") {
        return std::make_shared<VoxelIndexAttribute>();
    } else if (name == "voxel_radius_edge_ratio") {
        return std::make_shared<VoxelRadiusEdgeRatioAttribute>();
    } else if (name == "voxel_volume") {
        return std::make_shared<VoxelVolumeAttribute>();
    } else {
        // Generic attribute.
        return std::make_shared<MeshAttribute>();
    }
}
