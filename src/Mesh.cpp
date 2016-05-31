/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Mesh.h"

#include <iostream>

#include <Attributes/MeshAttributes.h>
#include <Connectivity/MeshConnectivity.h>
#include <Core/EigenTypedef.h>
#include <Geometry/MeshGeometry.h>

using namespace PyMesh;

Mesh::Mesh() {
    m_geometry     = GeometryPtr    (new MeshGeometry());
    m_connectivity = ConnectivityPtr(new MeshConnectivity());
    m_attributes   = AttributesPtr  (new MeshAttributes());
}

size_t Mesh::get_dim() const {
    return m_geometry->get_dim();
}

size_t Mesh::get_num_vertices() const {
    return m_geometry->get_num_vertices();
}

size_t Mesh::get_num_faces() const {
    return m_geometry->get_num_faces();
}

size_t Mesh::get_num_voxels() const {
    return m_geometry->get_num_voxels();
}

VectorF Mesh::get_vertex(size_t i) {
    const size_t dim = get_dim();
    return get_vertices().segment(i*dim, dim);
}

VectorI Mesh::get_face(size_t i) {
    size_t stride = get_vertex_per_face();
    return get_faces().segment(i*stride, stride);
}

VectorI Mesh::get_voxel(size_t i) {
    size_t stride = get_vertex_per_voxel();
    return get_voxels().segment(i*stride, stride);
}

VectorF Mesh::get_vertex(size_t i) const {
    const size_t dim = get_dim();
    return get_vertices().segment(i*dim, dim);
}

VectorI Mesh::get_face(size_t i) const {
    size_t stride = get_vertex_per_face();
    return get_faces().segment(i*stride, stride);
}

VectorI Mesh::get_voxel(size_t i) const {
    size_t stride = get_vertex_per_voxel();
    return get_voxels().segment(i*stride, stride);
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

const VectorF& Mesh::get_vertices() const {
    return m_geometry->get_vertices();
}

const VectorI& Mesh::get_faces() const {
    return m_geometry->get_faces();
}

const VectorI& Mesh::get_voxels() const {
    return m_geometry->get_voxels();
}

int Mesh::get_vertex_per_face() const {
    return m_geometry->get_vertex_per_face();
}

int Mesh::get_vertex_per_voxel() const {
    return m_geometry->get_vertex_per_voxel();
}

void Mesh::enable_connectivity() {
    enable_vertex_connectivity();
    enable_face_connectivity();
    enable_voxel_connectivity();
}

void Mesh::enable_vertex_connectivity() {
    m_connectivity->init_vertex_adjacencies(this);
}

void Mesh::enable_face_connectivity() {
    m_connectivity->init_face_adjacencies(this);
}

void Mesh::enable_voxel_connectivity() {
    m_connectivity->init_voxel_adjacencies(this);
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

bool Mesh::has_attribute(const std::string& attr_name) const {
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

const VectorF& Mesh::get_attribute(const std::string& attr_name) const {
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
