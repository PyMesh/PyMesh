/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ElementWiseMaterial.h"

#include <algorithm>
#include <cassert>

using namespace PyMesh;

ElementWiseMaterial::ElementWiseMaterial(Float density, MeshPtr material_mesh) :
    m_density(density), m_material_mesh(material_mesh) {
        const size_t dim = m_material_mesh->get_dim();
        if (dim == 2) {
            initialize_2D_grid();
        } else if (dim == 3) {
            initialize_3D_grid();
        } else {
            throw NotImplementedError("Only 2D and 3D meshes are supported.");
        }
}

Float ElementWiseMaterial::get_material_tensor(
        size_t i, size_t j, size_t k, size_t l, VectorF coord) const {
    const VectorI voxel_ids = look_up_voxels(coord);
    assert(voxel_ids.size() > 0);
    const size_t voxel_id = voxel_ids[0];
    assert(voxel_id < m_materials.size());
    return m_materials[voxel_id]->get_material_tensor(i,j,k,l,coord);
}

MatrixF ElementWiseMaterial::strain_to_stress(
        const MatrixF& strain, VectorF coord) const {
    const VectorI voxel_ids = look_up_voxels(coord);
    assert(voxel_ids.size() > 0);
    const size_t voxel_id = voxel_ids[0];
    assert(voxel_id < m_materials.size());
    return m_materials[voxel_id]->strain_to_stress(strain, coord);
}

VectorI ElementWiseMaterial::look_up_voxels(const VectorF& coords) const {
    //const size_t dim = m_material_mesh->get_dim();
    //const size_t length = coords.size();
    //const size_t num_coords = length / dim;
    //Eigen::Map<const MatrixFr> unflattened_coords(coords.data(), num_coords, dim);

    //m_locator.locate(unflattened_coords);
    //return m_locator.get_enclosing_voxels();

    VectorI candidates = m_grid->get_items_near_point(coords);
    return candidates;
}

Float ElementWiseMaterial::compute_cell_size() {
    if (!m_material_mesh->has_attribute("edge_length")) {
        m_material_mesh->add_attribute("edge_length");
    }
    const VectorF& edge_lengths = m_material_mesh->get_attribute("edge_length");
    return 0.1 * edge_lengths.sum() / edge_lengths.size();
}

void ElementWiseMaterial::initialize_2D_grid() {
    const size_t dim = m_material_mesh->get_dim();
    const size_t num_elem = m_material_mesh->get_num_faces();
    const Float cell_size = compute_cell_size();

    m_grid = HashGrid::create(cell_size, 2);

    if (!m_material_mesh->has_attribute("face_centroid")) {
        m_material_mesh->add_attribute("face_centroid");
    }
    const VectorF& keys = m_material_mesh->get_attribute("face_centroid");

    for (size_t i=0; i<num_elem; i++) {
        m_grid->insert(i, keys.segment(i*dim, dim));
    }
}

void ElementWiseMaterial::initialize_3D_grid() {
    const size_t dim = m_material_mesh->get_dim();
    const size_t num_elem = m_material_mesh->get_num_voxels();
    const Float cell_size = compute_cell_size();

    m_grid = HashGrid::create(cell_size, 3);

    if (!m_material_mesh->has_attribute("voxel_centroid")) {
        m_material_mesh->add_attribute("voxel_centroid");
    }
    const VectorF& keys = m_material_mesh->get_attribute("voxel_centroid");

    for (size_t i=0; i<num_elem; i++) {
        m_grid->insert(i, keys.segment(i*dim, dim));
    }
}
