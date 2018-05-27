/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "DracoCompressionEngine.h"

#include <Core/Exception.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <draco/attributes/geometry_attribute.h>
#include <draco/compression/encode.h>
#include <draco/compression/decode.h>
#include <draco/mesh/mesh.h>

#include <cassert>
#include <memory>
#include <sstream>

using namespace PyMesh;

namespace DracoCompressionEngineHelper {

std::unique_ptr<draco::Mesh> to_draco_mesh(Mesh::Ptr mesh) {
    std::unique_ptr<draco::Mesh> draco_mesh(new draco::Mesh());

    const size_t dim = mesh->get_dim();
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();

    if (vertex_per_face != 3) {
        throw NotImplementedError(
                "Draco encoding only supports triangle mesh.");
    }

    draco_mesh->set_num_points(num_vertices);
    draco::GeometryAttribute positions;
    positions.Init(draco::GeometryAttribute::POSITION, // Attribute type
            nullptr,                                   // data buffer
            dim,                                       // number of components
            draco::DT_FLOAT64,                         // data type
            false,                                     // normalized
            sizeof(Float) * dim,                       // byte stride
            0);                                        // byte offset
    auto pos_att_id = draco_mesh->AddAttribute(
            positions,      // attribute object
            true,           // identity mapping
            num_vertices);  // num attribute values

    for (int i = 0; i < num_vertices; ++i) {
        draco_mesh->attribute(pos_att_id)->SetAttributeValue(
                draco::AttributeValueIndex(i), mesh->get_vertex(i).data());
    }

    const auto& faces = mesh->get_faces();
    for (int i = 0; i < num_faces; ++i) {
        draco_mesh->AddFace({{
                draco::PointIndex(faces[i*3]),
                draco::PointIndex(faces[i*3+1]),
                draco::PointIndex(faces[i*3+2])
                }});
    }

    return draco_mesh;
}

Mesh::Ptr from_draco_mesh(std::unique_ptr<draco::Mesh> draco_mesh) {
    assert(draco_mesh);
    const auto num_vertices = draco_mesh->num_points();
    const auto positions = draco_mesh->GetNamedAttribute(
            draco::GeometryAttribute::POSITION);
    assert(positions->IsValid());
    const auto dim = positions->num_components();

    VectorF vertices(num_vertices * dim);
    for (size_t i=0; i<num_vertices; i++) {
        const auto p = positions->GetValue<Float, 3>(
                draco::AttributeValueIndex(i));
        for (size_t j=0; j<dim; j++) {
            vertices(i*dim+j) = p[j];
        }
    }

    const auto num_faces = draco_mesh->num_faces();
    VectorI faces(num_faces * 3);
    for (size_t i=0; i<num_faces; i++) {
        const auto f = draco_mesh->face(draco::FaceIndex(i));
        faces.segment<3>(i*3) << f[0].value(), f[1].value(), f[2].value();
    }

    VectorI voxels;
    return MeshFactory().load_data(vertices, faces, voxels, dim, 3, 4).create();
}

Mesh::Ptr from_draco_point_cloud(std::unique_ptr<draco::PointCloud> draco_pc) {
    assert(draco_pc);
    const auto num_vertices = draco_pc->num_points();
    const auto positions = draco_pc->GetNamedAttribute(
            draco::GeometryAttribute::POSITION);
    assert(positions->IsValid());
    const auto dim = positions->num_components();

    VectorF vertices(num_vertices * dim);
    for (size_t i=0; i<num_vertices; i++) {
        const auto p = positions->GetValue<Float, 3>(
                draco::AttributeValueIndex(i));
        for (size_t j=0; j<dim; j++) {
            vertices(i*dim+j) = p[j];
        }
    }

    VectorI faces;
    VectorI voxels;
    return MeshFactory().load_data(vertices, faces, voxels, dim, 3, 4).create();
}

}

std::string DracoCompressionEngine::compress(Mesh::Ptr mesh) const {
    auto draco_mesh = DracoCompressionEngineHelper::to_draco_mesh(mesh);
    draco::EncoderBuffer buffer;
    draco::Encoder encoder;
    const auto status = encoder.EncodeMeshToBuffer(*draco_mesh, &buffer);
    if (!status.ok()) {
        throw RuntimeError("Draco encoding error!");
    }
    return std::string(buffer.data(), buffer.size());
}

Mesh::Ptr DracoCompressionEngine::decompress(const std::string& data) const {
    draco::DecoderBuffer buffer;
    buffer.Init(data.c_str(), data.size());
    auto type_statusor = draco::Decoder::GetEncodedGeometryType(&buffer);
    if (!type_statusor.ok()) {
        throw RuntimeError("Failed to decode Draco buffer.");
    }

    draco::Decoder decoder;
    draco::Mesh* draco_mesh = nullptr;
    const draco::EncodedGeometryType geom_type = type_statusor.value();
    if (geom_type == draco::TRIANGULAR_MESH) {
        auto statusor = decoder.DecodeMeshFromBuffer(&buffer);
        if (!statusor.ok()) {
            throw RuntimeError("Draco decoding from triangle mesh failed.");
        }
        std::unique_ptr<draco::Mesh> in_mesh = std::move(statusor).value();
        if (!in_mesh) {
            throw RuntimeError("Draco decoding from triangle mesh failed.");
        }
        return DracoCompressionEngineHelper::from_draco_mesh(std::move(in_mesh));
    } else if (geom_type == draco::POINT_CLOUD) {
        auto statusor = decoder.DecodeMeshFromBuffer(&buffer);
        if (!statusor.ok()) {
            throw RuntimeError("Draco decoding from triangle mesh failed.");
        }
        std::unique_ptr<draco::PointCloud> in_mesh = std::move(statusor).value();
        if (!in_mesh) {
            throw RuntimeError("Draco decoding from point cloud failed.");
        }
        return DracoCompressionEngineHelper::from_draco_point_cloud(std::move(in_mesh));
    } else {
        throw NotImplementedError("Unsupported Draco mesh type.");
    }
}
