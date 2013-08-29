#include "Mesh.h"

#include <tr1/memory>

#include "EigenTypedef.h"
#include "MeshAttributes.h"
#include "MeshConnectivity.h"
#include "MeshGeometry.h"

Mesh::Mesh() {
    m_geometry     = GeometryPtr    (new MeshGeometry());
    m_connectivity = ConnectivityPtr(new MeshConnectivity());
    m_attributes   = AttributesPtr  (new MeshAttributes());
}

Vector3F Mesh::get_vertex(size_t i) {
    return get_vertices().segment<3>(i*3);
}

Vector3I Mesh::get_face(size_t i) {
    return get_faces().segment<3>(i*3);
}

VectorI Mesh::get_voxel(size_t i) {
    return get_voxels().segment(i, get_vertex_per_voxel());
}

VectorF& Mesh::get_vertices() {
    return m_geometry->get_vertices();
}

VectorI& Mesh::get_faces() {
    return m_geometry->get_faces();
}

VectorI& Mesh::get_voxels() {
    return m_geometry->get_voxels();
}

int Mesh::get_vertex_per_face() const {
    return m_geometry->get_vertex_per_face();
}

int Mesh::get_vertex_per_voxel() const {
    return m_geometry->get_vertex_per_voxel();
}


VectorI Mesh::get_vertex_adjacent_vertices(size_t vi) const {
    return m_connectivity->get_vertex_adjacent_vertices(vi);
}

VectorI Mesh::get_vertex_adjacent_faces(size_t vi) const {
    return m_connectivity->get_vertex_adjacent_faces(vi);
}

VectorI Mesh::get_vertex_adjacent_voxels(size_t vi) const {
    return m_connectivity->get_vertex_adjacent_voxels(vi);
}

VectorI Mesh::get_face_adjacent_faces(size_t fi) const {
    return m_connectivity->get_face_adjacent_faces(fi);
}

VectorI Mesh::get_face_adjacent_voxels(size_t fi) const {
    return m_connectivity->get_face_adjacent_voxels(fi);
}

VectorI Mesh::get_voxel_adjacent_faces(size_t Vi) const {
    return m_connectivity->get_voxel_adjacent_faces(Vi);
}

VectorI Mesh::get_voxel_adjacent_voxels(size_t Vi) const {
    return m_connectivity->get_voxel_adjacent_voxels(Vi);
}

bool Mesh::has_attribute(const std::string& attr_name) {
    return m_attributes->has_attribute(attr_name);
}

void Mesh::add_attribute(const std::string& attr_name) {
    m_attributes->add_attribute(attr_name, *this);
}

void Mesh::remove_attribute(const std::string& attr_name) {
    m_attributes->remove_attribute(attr_name);
}

VectorF& Mesh::get_attribute(const std::string& attr_name) {
    return m_attributes->get_attribute(attr_name);
}

void Mesh::set_attribute(const std::string& attr_name, VectorF& attr_value) {
    return m_attributes->set_attribute(attr_name, attr_value);
}

std::vector<std::string> Mesh::get_attribute_names() const {
    return m_attributes->get_attribute_names();
}

void Mesh::set_geometry(GeometryPtr geometry) {
    m_geometry = geometry;
}

void Mesh::set_connectivity(ConnectivityPtr connectivity) {
    m_connectivity = connectivity;
}

void Mesh::set_attributes(AttributesPtr attributes) {
    m_attributes = attributes;
}
