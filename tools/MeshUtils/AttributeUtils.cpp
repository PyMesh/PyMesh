#include "AttributeUtils.h"

#include <map>
#include <iostream>

#include "Mesh.h"
#include "Exception.h"
#include "Triplet.h"

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
    if (!mesh.has_attribute("voxel_volume")) {
        mesh.add_attribute("voxel_volume");
    }
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

VectorF convert_voxel_attribute_to_face_attribute(
        Mesh& mesh, const VectorF& attribute) {
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_voxels = mesh.get_num_voxels();
    const size_t attr_size = attribute.size();
    const size_t stride = attr_size / num_voxels;
    const size_t vertex_per_voxel = mesh.get_vertex_per_voxel();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    if (vertex_per_voxel != 4)
        throw NotImplementedError("Voxel type is not yet supported");
    if (vertex_per_face != 3)
        throw NotImplementedError("Face type is not yet supported");

    const VectorI& faces = mesh.get_faces();
    const VectorI& voxels = mesh.get_voxels();

    std::map<Triplet, VectorF> per_face_values;
    for (size_t i=0; i<num_voxels; i++) {
        const VectorI& voxel = voxels.segment(
                i*vertex_per_voxel, vertex_per_voxel);
        const VectorF& value = attribute.segment(
                i*stride, stride);
        per_face_values[Triplet(voxel[0], voxel[1], voxel[2])] = value;
        per_face_values[Triplet(voxel[0], voxel[1], voxel[3])] = value;
        per_face_values[Triplet(voxel[0], voxel[2], voxel[3])] = value;
        per_face_values[Triplet(voxel[1], voxel[2], voxel[3])] = value;
    }

    VectorF result = VectorF::Zero(num_faces*stride);
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& face = faces.segment(i*vertex_per_face, vertex_per_face);
        result.segment(i*stride, stride) = per_face_values[
            Triplet(face[0], face[1], face[2])];
    }

    return result;
}

VectorF Zhou::convert_to_vertex_attribute(Mesh& mesh, const VectorF& attribute) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_voxels = mesh.get_num_voxels();

    size_t attr_size = attribute.size();

    if (attr_size == num_vertices || attr_size == dim*num_vertices) {
        std::cerr << "Warning: attribute is already face attribute!  "
            << "This copy is unnecessary!" << std::endl;
        return attribute;
    } else if (attr_size == num_faces || attr_size == dim*num_faces) {
        return convert_face_attribute_to_vertex_attribute(mesh, attribute);
    } else if (attr_size == num_voxels || attr_size == dim*num_voxels) {
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

VectorF Zhou::convert_to_face_attribute(Mesh& mesh, const VectorF& attribute) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t num_voxels = mesh.get_num_voxels();

    size_t attr_size = attribute.size();

    if (attr_size == num_vertices || attr_size == dim*num_vertices) {
        throw NotImplementedError(
                "Converting vertex attribute to face attribute is not supported yet.");
    } else if (attr_size == num_faces || attr_size == dim*num_faces) {
        std::cerr << "Warning: attribute is already face attribute!  "
            << "This copy is unnecessary!" << std::endl;
        return attribute;
    } else if (attr_size == num_voxels || attr_size == dim*num_voxels) {
        return convert_voxel_attribute_to_face_attribute(mesh, attribute);
    } else {
        throw RuntimeError("Unknow attribute type.");
    }
}

VectorF Zhou::convert_to_face_attribute(Mesh& mesh,
        const std::string& attribute_name) {
    const VectorF& attr = mesh.get_attribute(attribute_name);
    return convert_to_face_attribute(mesh, attr);
}
