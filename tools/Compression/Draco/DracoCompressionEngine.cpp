/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_DRACO

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

template <typename DracoMesh>
void copy_vertices(Mesh::Ptr mesh, std::unique_ptr<DracoMesh>& draco_mesh) {
    const auto dim = mesh->get_dim();
    const auto num_vertices = mesh->get_num_vertices();
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
}

template <typename DracoMesh>
void copy_faces(Mesh::Ptr mesh, std::unique_ptr<DracoMesh>& draco_mesh) {
    const auto num_faces = mesh->get_num_faces();
    const auto& faces = mesh->get_faces();
    for (int i = 0; i < num_faces; ++i) {
        draco_mesh->AddFace({{
                draco::PointIndex(faces[i*3]),
                draco::PointIndex(faces[i*3+1]),
                draco::PointIndex(faces[i*3+2])
                }});
    }
}

template <typename DracoMesh>
void copy_vertex_attributes(Mesh::Ptr mesh,
        std::unique_ptr<DracoMesh>& draco_mesh) {
    const auto num_vertices = mesh->get_num_vertices();
    const auto& attribute_names = mesh->get_attribute_names();
    for (const auto& name : attribute_names) {
        const auto& values = mesh->get_attribute(name);
        if (values.size() % num_vertices != 0) continue;
        const auto num_rows = num_vertices;
        const auto num_cols = values.size() / num_vertices;
        draco::GeometryAttribute attr;
        if (name == "vertex_normal") {
            attr.Init(draco::GeometryAttribute::NORMAL, nullptr,
                    num_cols, draco::DT_FLOAT64, false,
                    sizeof(Float) * num_cols, 0);
        } else if (name == "vertex_texture") {
            attr.Init(draco::GeometryAttribute::TEX_COORD, nullptr,
                    num_cols, draco::DT_FLOAT64, false,
                    sizeof(Float) * num_cols, 0);
        } else if (name.substr(0, 6) == "vertex"){
            attr.Init(draco::GeometryAttribute::GENERIC, nullptr,
                    num_cols, draco::DT_FLOAT64, false,
                    sizeof(Float) * num_cols, 0);
        } else {
            // Not a vertex attribute.
            continue;
        }
        const auto id = draco_mesh->AddAttribute(attr, true, num_rows);
        for (size_t i=0; i<num_rows; i++) {
            draco_mesh->attribute(id)->SetAttributeValue(
                    draco::AttributeValueIndex(i), values.data() +i*num_cols);
        }

        std::unique_ptr<draco::AttributeMetadata> metadata =
            std::make_unique<draco::AttributeMetadata>();
        metadata->AddEntryString("name", name);
        draco_mesh->AddAttributeMetadata(id, std::move(metadata));
    }
}

void copy_face_attributes(Mesh::Ptr mesh,
        std::unique_ptr<draco::Mesh>& draco_mesh) {
    const auto num_faces = mesh->get_num_faces();
    const auto& attribute_names = mesh->get_attribute_names();
    for (const auto& name : attribute_names) {
        const auto& values = mesh->get_attribute(name);
        if (values.size() % num_faces != 0) continue;
        const auto num_rows = num_faces;
        const auto num_cols = values.size() / num_faces;
        draco::GeometryAttribute attr;
        if (name == "face_normal") {
            attr.Init(draco::GeometryAttribute::NORMAL, nullptr,
                    num_cols, draco::DT_FLOAT64, false,
                    sizeof(Float) * num_cols, 0);
        } else if (name.substr(0, 4) == "face"){
            attr.Init(draco::GeometryAttribute::GENERIC, nullptr,
                    num_cols, draco::DT_FLOAT64, false,
                    sizeof(Float) * num_cols, 0);
        } else {
            // Not a face attribute.
            continue;
        }
        const auto id = draco_mesh->AddAttribute(attr, true, num_rows);
        draco_mesh->SetAttributeElementType(id, draco::MESH_FACE_ATTRIBUTE);
        for (size_t i=0; i<num_rows; i++) {
            draco_mesh->attribute(id)->SetAttributeValue(
                    draco::AttributeValueIndex(i), values.data() + i*num_cols);
        }

        std::unique_ptr<draco::AttributeMetadata> metadata =
            std::make_unique<draco::AttributeMetadata>();
        metadata->AddEntryString("name", name);
        draco_mesh->AddAttributeMetadata(id, std::move(metadata));
    }
}

std::unique_ptr<draco::Mesh> to_draco_mesh(Mesh::Ptr mesh,
        bool with_attributes=true) {
    std::unique_ptr<draco::Mesh> draco_mesh(new draco::Mesh());

    const size_t vertex_per_face = mesh->get_vertex_per_face();
    if (vertex_per_face != 3) {
        throw NotImplementedError(
                "Draco encoding only supports triangle mesh.");
    }

    copy_vertices(mesh, draco_mesh);
    copy_faces(mesh, draco_mesh);

    if (with_attributes) {
        copy_vertex_attributes(mesh, draco_mesh);
        //copy_face_attributes(mesh, draco_mesh);
    }

    return draco_mesh;
}

std::unique_ptr<draco::PointCloud> to_draco_point_cloud(Mesh::Ptr mesh,
        bool with_attributes=true) {
    std::unique_ptr<draco::PointCloud> draco_mesh(new draco::PointCloud());
    assert(mesh->get_num_faces() == 0);
    copy_vertices(mesh, draco_mesh);

    if (with_attributes) {
        copy_vertex_attributes(mesh, draco_mesh);
    }

    return draco_mesh;
}

template<typename DracoMesh>
VectorF extract_vertices(std::unique_ptr<DracoMesh>& draco_mesh, size_t& dim) {
    const auto num_vertices = draco_mesh->num_points();
    const auto positions = draco_mesh->GetNamedAttribute(
            draco::GeometryAttribute::POSITION);
    assert(positions->IsValid());
    dim = positions->num_components();

    VectorF vertices(num_vertices * dim);
    if (dim == 2) {
        for (size_t i=0; i<num_vertices; i++) {
            const auto p = positions->template GetValue<Float, 2>(
                    draco::AttributeValueIndex(i));
            vertices(i*2  ) = p[0];
            vertices(i*2+1) = p[1];
        }
    } else if (dim == 3) {
        for (size_t i=0; i<num_vertices; i++) {
            const auto p = positions->template GetValue<Float, 3>(
                    draco::AttributeValueIndex(i));
            vertices(i*3  ) = p[0];
            vertices(i*3+1) = p[1];
            vertices(i*3+2) = p[2];
        }
    } else {
        throw NotImplementedError("Draco mesh encodes high dimensional data");
    }
    return vertices;
}

template<typename DracoMesh>
VectorI extract_faces(std::unique_ptr<DracoMesh>& draco_mesh) {
    const auto num_faces = draco_mesh->num_faces();
    VectorI faces(num_faces * 3);
    for (size_t i=0; i<num_faces; i++) {
        const auto f = draco_mesh->face(draco::FaceIndex(i));
        faces.segment<3>(i*3) << f[0].value(), f[1].value(), f[2].value();
    }
    return faces;
}

template<typename DracoMesh>
void copy_metadata(std::unique_ptr<DracoMesh>& draco_mesh, Mesh::Ptr mesh) {
    const auto metadata = draco_mesh->GetMetadata();
    if (metadata == nullptr) return;
    const auto& attr_metadatas = metadata->attribute_metadatas();
    for (const auto& attr_metadata : attr_metadatas) {
        std::string name="";
        attr_metadata->GetEntryString("name", &name);
        if (name == "") continue;
        auto uid = attr_metadata->att_unique_id();

        const auto attr = draco_mesh->GetAttributeByUniqueId(uid);
        const auto num_rows = attr->size();
        const auto num_cols = attr->num_components();

        VectorF data(num_rows * num_cols);
        for (size_t i=0; i<num_rows; i++) {
            attr->ConvertValue(draco::AttributeValueIndex(i),
                    data.data() + i*num_cols);
        }
        mesh->add_empty_attribute(name);
        mesh->set_attribute(name, data);
    }
}

Mesh::Ptr from_draco_mesh(std::unique_ptr<draco::Mesh> draco_mesh) {
    assert(draco_mesh);
    size_t dim;
    VectorF vertices = extract_vertices(draco_mesh, dim);
    VectorI faces = extract_faces(draco_mesh);
    VectorI voxels;
    auto mesh = MeshFactory().load_data(vertices, faces, voxels, dim, 3, 4).create();
    copy_metadata(draco_mesh, mesh);
    return mesh;
}

Mesh::Ptr from_draco_point_cloud(std::unique_ptr<draco::PointCloud> draco_pc) {
    assert(draco_pc);
    size_t dim;
    VectorF vertices = extract_vertices(draco_pc, dim);
    VectorI faces;
    VectorI voxels;
    auto mesh = MeshFactory().load_data(vertices, faces, voxels, dim, 3, 4).create();
    copy_metadata(draco_pc, mesh);
    return mesh;
}

}

std::string DracoCompressionEngine::compress(Mesh::Ptr mesh) const {
    const size_t num_faces = mesh->get_num_faces();

    draco::EncoderBuffer buffer;
    draco::Encoder encoder;
    if (num_faces > 0) {
        auto draco_mesh = DracoCompressionEngineHelper::to_draco_mesh(mesh);
        const auto status = encoder.EncodeMeshToBuffer(*draco_mesh, &buffer);
        if (!status.ok()) {
            throw RuntimeError("Draco encoding error!");
        }
        return std::string(buffer.data(), buffer.size());
    } else {
        auto draco_mesh = DracoCompressionEngineHelper::to_draco_point_cloud(mesh);
        const auto status = encoder.EncodePointCloudToBuffer(*draco_mesh, &buffer);
        if (!status.ok()) {
            throw RuntimeError("Draco encoding error!");
        }
        return std::string(buffer.data(), buffer.size());
    }
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
        auto statusor = decoder.DecodePointCloudFromBuffer(&buffer);
        if (!statusor.ok()) {
            throw RuntimeError("Draco decoding from point cloud failed.");
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

#endif
