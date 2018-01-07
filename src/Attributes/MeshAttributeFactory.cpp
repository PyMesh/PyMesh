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
        return std::make_shared<VertexNormalAttribute>(name);
    } else if (name == "vertex_volume") {
        return std::make_shared<VertexVolumeAttribute>(name);
    } else if (name == "vertex_area") {
        return std::make_shared<VertexAreaAttribute>(name);
    } else if (name == "vertex_laplacian") {
        return std::make_shared<VertexLaplacianAttribute>(name);
    } else if (name == "vertex_mean_curvature") {
        return std::make_shared<VertexMeanCurvatureAttribute>(name);
    } else if (name == "vertex_gaussian_curvature") {
        return std::make_shared<VertexGaussianCurvatureAttribute>(name);
    } else if (name == "vertex_index") {
        return std::make_shared<VertexIndexAttribute>(name);
    } else if (name == "vertex_valance") {
        return std::make_shared<VertexValanceAttribute>(name);
    } else if (name == "vertex_dihedral_angle") {
        return std::make_shared<VertexDihedralAngleAttribute>(name);
    } else if (name == "vertex_voronoi_area") {
        return std::make_shared<VertexVoronoiAreaAttribute>(name);
    } else if (name == "edge_length") {
        return std::make_shared<EdgeLengthAttribute>(name);
    } else if (name == "edge_squared_length") {
        return std::make_shared<EdgeSquaredLengthAttribute>(name);
    } else if (name == "edge_dihedral_angle") {
        return std::make_shared<EdgeDihedralAngleAttribute>(name);
    } else if (name == "face_area") {
        return std::make_shared<FaceAreaAttribute>(name);
    } else if (name == "face_aspect_ratio") {
        return std::make_shared<FaceAspectRatioAttribute>(name);
    } else if (name == "face_centroid") {
        return std::make_shared<FaceCentroidAttribute>(name);
    } else if (name == "face_circumcenter") {
        return std::make_shared<FaceCircumCenterAttribute>(name);
    } else if (name == "face_circumradius") {
        return std::make_shared<FaceCircumRadiusAttribute>(name);
    } else if (name == "face_edge_ratio") {
        return std::make_shared<FaceEdgeRatioAttribute>(name);
    } else if (name == "face_incircle_center") {
        return std::make_shared<FaceIncircleCenterAttribute>(name);
    } else if (name == "face_incircle_radius") {
        return std::make_shared<FaceIncircleRadiusAttribute>(name);
    } else if (name == "face_index") {
        return std::make_shared<FaceIndexAttribute>(name);
    } else if (name == "face_normal") {
        return std::make_shared<FaceNormalAttribute>(name);
    } else if (name == "face_voronoi_area") {
        return std::make_shared<FaceVoronoiAreaAttribute>(name);
    } else if (name == "voxel_dihedral_angle") {
        return std::make_shared<VoxelDihedralAngleAttribute>(name);
    } else if (name == "voxel_edge_ratio") {
        return std::make_shared<VoxelEdgeRatioAttribute>(name);
    } else if (name == "voxel_face_index") {
        return std::make_shared<VoxelFaceIndexAttribute>(name);
    } else if (name == "voxel_centroid") {
        return std::make_shared<VoxelCentroidAttribute>(name);
    } else if (name == "voxel_circumcenter") {
        return std::make_shared<VoxelCircumCenterAttribute>(name);
    } else if (name == "voxel_circumradius") {
        return std::make_shared<VoxelCircumRadiusAttribute>(name);
    } else if (name == "voxel_incenter") {
        return std::make_shared<VoxelIncenterAttribute>(name);
    } else if (name == "voxel_inradius") {
        return std::make_shared<VoxelInradiusAttribute>(name);
    } else if (name == "voxel_index") {
        return std::make_shared<VoxelIndexAttribute>(name);
    } else if (name == "voxel_radius_edge_ratio") {
        return std::make_shared<VoxelRadiusEdgeRatioAttribute>(name);
    } else if (name == "voxel_volume") {
        return std::make_shared<VoxelVolumeAttribute>(name);
    } else {
        // Generic attribute.
        return std::make_shared<MeshAttribute>(name);
    }
}
