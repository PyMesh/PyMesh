#include "AttributeUtils.h"

#include "Mesh.h"
#include "Exception.h"

using namespace Zhou;

VectorF convert_face_attribute_to_vertex_attribute(
        Mesh& mesh, const VectorF& attribute) {
    const size_t num_vertices = mesh.get_vertices().size() / 3;
    const size_t vertex_per_face = mesh.get_vertex_per_face();
    const size_t num_faces = mesh.get_faces().size() / vertex_per_face;
    const size_t attr_size = attribute.size();
    const size_t stride = attr_size / num_faces;

    const VectorI& faces = mesh.get_faces();
    const VectorF& weights = mesh.get_attribute("face_area");

    VectorF result = VectorF::Zero(num_vertices * stride);
    VectorF result_weights = VectorF::Zero(num_vertices);
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& face =
            faces.segment(i*vertex_per_face, vertex_per_face);
        Float per_vertex_weight = weights[i] / vertex_per_face;
        for (size_t j=0; j<vertex_per_face; j++) {
            result_weights[face[j]] += per_vertex_weight;
            result.segment(face[j]*stride, stride) +=
                per_vertex_weight * attribute.segment(i*stride, stride);
        }
    }

    for (size_t i=0; i<num_vertices; i++) {
        result.segment(i*stride, stride) /= result_weights[i];
    }
    return result;
}

VectorF convert_voxel_attribute_to_vertex_attribute(
        Mesh& mesh, const VectorF& attribute) {
    const size_t num_vertices = mesh.get_vertices().size() / 3;
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    const size_t num_voxels = mesh.get_voxels().size() / vertex_per_voxel;
    const size_t attr_size = attribute.size();
    const size_t stride = attr_size / num_voxels;

    const VectorI& voxels = mesh.get_voxels();
    const VectorF& weights = mesh.get_attribute("voxel_volume");

    VectorF result = VectorF::Zero(num_vertices * stride);
    VectorF result_weights = VectorF::Zero(num_vertices);
    for (size_t i=0; i<num_voxels; i++) {
        const VectorI& voxel =
            voxels.segment(i*vertex_per_voxel, vertex_per_voxel);
        Float per_vertex_weight = weights[i] / vertex_per_voxel;
        for (size_t j=0; j<vertex_per_voxel; j++) {
            result_weights[voxel[j]] += per_vertex_weight;
            result.segment(voxel[j]*stride, stride) +=
                per_vertex_weight * attribute.segment(i*stride, stride);
        }
    }

    for (size_t i=0; i<num_vertices; i++) {
        result.segment(i*stride, stride) /= result_weights[i];
    }
    return result;
}

VectorF Zhou::convert_to_vertex_attribute(Mesh& mesh, const VectorF& attribute) {
    const size_t vertex_per_face = mesh.get_vertex_per_face();
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    const size_t num_vertices = mesh.get_vertices().size() / 3;
    const size_t num_faces = mesh.get_faces().size() / vertex_per_face;
    const size_t num_voxels = (vertex_per_voxel == 0) ? 0 :
        mesh.get_voxels().size() / vertex_per_voxel;

    size_t attr_size = attribute.size();

    if (attr_size == num_vertices || attr_size == 3*num_vertices) {
        return attribute;
    } else if (attr_size == num_faces || attr_size == 3*num_faces) {
        return convert_face_attribute_to_vertex_attribute(mesh, attribute);
    } else if (attr_size == num_voxels || attr_size == 3*num_voxels) {
        return convert_voxel_attribute_to_vertex_attribute(mesh, attribute);
    } else {
        throw RuntimeError("Unknow attribute type.");
    }
}

VectorF Zhou::convert_to_vertex_attribute(Mesh& mesh,
        const std::string& attribute_name) {
    const VectorF& attr = mesh.get_attribute(attribute_name);
    return convert_to_vertex_attribute(mesh, attr);
}
