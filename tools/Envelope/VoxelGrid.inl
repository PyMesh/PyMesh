#include <cassert>
#include <sstream>
#include <functional>
#include <queue>

#include <Core/Exception.h>
#include <MeshFactory.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>

#include "Eroder.h"

using namespace PyMesh;

namespace VoxelGridHelper {
    bool nonzero(short i) { return i!=0; }

    template<int DIM>
    VectorI append_cell_corners(
            const typename VoxelGrid<DIM>::Vector_f& cell_center,
            const typename VoxelGrid<DIM>::Vector_f& half_cell_size,
            std::vector<typename VoxelGrid<DIM>::Vector_f>& vertices) {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << DIM;
        throw NotImplementedError(err_msg.str());
    }

    template<>
    VectorI append_cell_corners<2>(
            const typename VoxelGrid<2>::Vector_f& cell_center,
            const typename VoxelGrid<2>::Vector_f& half_cell_size,
            std::vector<typename VoxelGrid<2>::Vector_f>& vertices) {
        typedef VoxelGrid<2>::Vector_f Vector_f;
        vertices.push_back(Vector_f(cell_center[0] - half_cell_size[0], cell_center[1] - half_cell_size[1]));
        vertices.push_back(Vector_f(cell_center[0] + half_cell_size[0], cell_center[1] - half_cell_size[1]));
        vertices.push_back(Vector_f(cell_center[0] + half_cell_size[0], cell_center[1] + half_cell_size[1]));
        vertices.push_back(Vector_f(cell_center[0] - half_cell_size[0], cell_center[1] + half_cell_size[1]));

        VectorI indices(4);
        indices << 0, 1, 2, 3;
        return indices;
    }

    template<>
    VectorI append_cell_corners<3>(
            const typename VoxelGrid<3>::Vector_f& cell_center,
            const typename VoxelGrid<3>::Vector_f& half_cell_size,
            std::vector<typename VoxelGrid<3>::Vector_f>& vertices) {
        typedef VoxelGrid<3>::Vector_f Vector_f;
        const Vector_f& c = cell_center;
        const Vector_f& h = half_cell_size;
        vertices.push_back(Vector_f(c[0]-h[0], c[1]-h[1], c[2]-h[2]));
        vertices.push_back(Vector_f(c[0]+h[0], c[1]-h[1], c[2]-h[2]));
        vertices.push_back(Vector_f(c[0]+h[0], c[1]+h[1], c[2]-h[2]));
        vertices.push_back(Vector_f(c[0]-h[0], c[1]+h[1], c[2]-h[2]));
        vertices.push_back(Vector_f(c[0]-h[0], c[1]-h[1], c[2]+h[2]));
        vertices.push_back(Vector_f(c[0]+h[0], c[1]-h[1], c[2]+h[2]));
        vertices.push_back(Vector_f(c[0]+h[0], c[1]+h[1], c[2]+h[2]));
        vertices.push_back(Vector_f(c[0]-h[0], c[1]+h[1], c[2]+h[2]));

        VectorI indices(8);
        indices << 0, 1, 2, 3, 4, 5, 6, 7;
        return indices;
    }

    Mesh::Ptr form_mesh(size_t dim, VectorF& nodes, VectorI& elements) {
        MeshFactory factory;
        if (dim == 2) {
            VectorI voxels;
            factory.load_data(nodes, elements, voxels, 2, 4, 0);
        } else if (dim == 3) {
            VectorI faces;
            factory.load_data(nodes, faces, elements, 3, 4, 8);
        }
        return factory.create_shared();
    }

    void remove_duplicated_vertices(MatrixFr& vertices, MatrixIr& elements) {
        DuplicatedVertexRemoval remover(vertices, elements);
        size_t num_duplicates = remover.run(1e-3);
        vertices = remover.get_vertices();
        elements = remover.get_faces();
    }
}

using namespace VoxelGridHelper;

template<int DIM>
VoxelGrid<DIM>::VoxelGrid(Float cell_size)
    : VoxelGrid<DIM>::Parent(cell_size), m_margin(1) {
        m_hash_grid = HashGrid::create(cell_size, DIM);
}

template<int DIM>
void VoxelGrid<DIM>::insert_mesh(Mesh::Ptr mesh) {
    if (mesh->get_dim() != DIM) {
        std::stringstream err_msg;
        err_msg << "Expect dim equals " << DIM << ", but mesh has dim " << mesh->get_dim();
        throw RuntimeError(err_msg.str());
    }
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    if (vertex_per_face == 3) {
        insert_triangle_mesh(mesh);
    } else if (vertex_per_face == 4) {
        insert_quad_mesh(mesh);
    } else {
        throw RuntimeError("Only triangle and quad meshes are supported!");
    }
}

template<int DIM>
void VoxelGrid<DIM>::create_grid() {
    MatrixFr centers = m_hash_grid->get_occupied_cell_centers();
    const size_t num_occupied_cells = centers.rows();

    VectorF box_min = centers.colwise().minCoeff();
    VectorF box_max = centers.colwise().maxCoeff();

    Vector_f grid_base_coord = box_min - this->m_cell_size;
    Vector_i grid_size = (box_max - box_min).cwiseQuotient(this->m_cell_size)
            .unaryExpr(std::ptr_fun<Float, Float>(std::round))
            .template cast<int>() + Vector_i::Ones() * (m_margin * 2 + 1);

    this->initialize(grid_size, grid_base_coord);

    for (size_t i=0; i<num_occupied_cells; i++) {
        this->lookup(centers.row(i)) += 1;
    }

    flood_exterior_cells();
}

template<int DIM>
void VoxelGrid<DIM>::dilate(size_t iterations) {
    // Dilate the solid regions is equivalent to erode the void reigions.
    // mask(idx) == true iff idx is void.
    Mask mask = create_mask();
    std::transform(mask.begin(), mask.end(), mask.begin(),
            std::logical_not<bool>());

    Eroder<DIM> eroder(mask);
    eroder.erode(iterations);

    mask = eroder.get_mask();

    auto mask_itr = mask.begin();
    auto data_itr = this->begin();
    for (; mask_itr != mask.end(); mask_itr++, data_itr++) {
        if (!*mask_itr) { *data_itr = 1; }
        else { *data_itr = 0; }
    }
    assert(data_itr == this->end());
}

template<int DIM>
void VoxelGrid<DIM>::erode(size_t iterations) {
    Mask solid = create_mask();

    Eroder<DIM> eroder(solid);
    eroder.erode(iterations);

    solid = eroder.get_mask();

    auto solid_itr = solid.begin();
    auto data_itr = this->begin();
    for (; solid_itr != solid.end(); solid_itr++, data_itr++) {
        if (*solid_itr) { *data_itr = 1; }
        else { *data_itr = 0; }
    }
    assert(data_itr == this->end());
}

template<int DIM>
Mesh::Ptr VoxelGrid<DIM>::get_voxel_mesh() {
    size_t index = 0;
    const Vector_f half_cell_size = this->m_cell_size * 0.5;

    std::vector<Vector_f> vertices;
    std::vector<VectorI> elements;

    size_t num_vertices = 0;
    for (auto itr=this->begin(); itr!=this->end(); itr++) {
        if (*itr == 0) continue;
        size_t i = itr - this->begin();
        Vector_f cell_center = this->get_cell_center(i);
        VectorI indices = append_cell_corners<DIM>(cell_center, half_cell_size, vertices);
        elements.push_back(indices + VectorI::Ones(indices.size()) * num_vertices);
        num_vertices += indices.size();
    }

    if (vertices.empty() || elements.empty()) {
        throw RuntimeError("Voxel grid does not contain any solid voxels.");
    }

    size_t num_elements = elements.size();

    MatrixFr mat_vertices(num_vertices, DIM);
    MatrixIr mat_elements(num_elements, elements[0].size());

    for (size_t i=0; i<num_vertices; i++) {
        mat_vertices.row(i) = vertices[i];
    }
    for (size_t i=0; i<num_elements; i++) {
        mat_elements.row(i) = elements[i];
    }
    remove_duplicated_vertices(mat_vertices, mat_elements);

    VectorF vec_vertices(mat_vertices.rows() * mat_vertices.cols());
    VectorI vec_elements(mat_elements.rows() * mat_elements.cols());
    std::copy(mat_vertices.data(), mat_vertices.data() + vec_vertices.size(),
            vec_vertices.data());
    std::copy(mat_elements.data(), mat_elements.data() + vec_elements.size(),
            vec_elements.data());
    return form_mesh(DIM, vec_vertices, vec_elements);
}

template<int DIM>
void VoxelGrid<DIM>::insert_triangle_mesh(Mesh::Ptr mesh) {
    const VectorF& vertices = mesh->get_vertices();
    const VectorI& faces = mesh->get_faces();
    const size_t num_faces = mesh->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        MatrixFr corners(3,DIM);
        const VectorI& face = faces.segment(i*3, 3);

        corners.row(0) = vertices.segment(face[0]*DIM, DIM);
        corners.row(1) = vertices.segment(face[1]*DIM, DIM);
        corners.row(2) = vertices.segment(face[2]*DIM, DIM);

        m_hash_grid->insert_triangle(i, corners);
    }
}

template<int DIM>
void VoxelGrid<DIM>::insert_quad_mesh(Mesh::Ptr mesh) {
    const VectorF& vertices = mesh->get_vertices();
    const VectorI& faces = mesh->get_faces();
    const size_t num_faces = mesh->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        MatrixFr corners(3,DIM);
        const VectorI& face = faces.segment(i*4, 4);

        //  3     2
        //   +---+
        //   | / |
        //   +---+
        //  0     1
        // Lower right triangle.
        corners.row(0) = vertices.segment(face[0]*DIM, DIM);
        corners.row(1) = vertices.segment(face[1]*DIM, DIM);
        corners.row(2) = vertices.segment(face[2]*DIM, DIM);
        m_hash_grid->insert_triangle(i, corners);

        // Upper left triangle.
        corners.row(0) = vertices.segment(face[0]*DIM, DIM);
        corners.row(1) = vertices.segment(face[2]*DIM, DIM);
        corners.row(2) = vertices.segment(face[3]*DIM, DIM);
        m_hash_grid->insert_triangle(i, corners);
    }
}

template<int DIM>
typename VoxelGrid<DIM>::Mask VoxelGrid<DIM>::create_mask() const {
    Mask mask(this->m_cell_size[0]);
    assert((mask.cell_size().array() == this->m_cell_size.array()).all());
    mask.initialize(this->size(), this->base_coordinates());
    std::transform(this->begin(), this->end(), mask.begin(),
            std::ptr_fun<short, bool>(nonzero));
    return mask;
}

template<int DIM>
void VoxelGrid<DIM>::flood_exterior_cells() {
    Mask mask = create_mask();

    flood_from_base_cell(mask);
    auto mask_itr = mask.begin();
    auto data_itr = this->begin();
    for (; mask_itr != mask.end(); mask_itr++, data_itr++) {
        if (!*mask_itr) { *data_itr = 1; }
    }
    assert(data_itr == this->end());
}

template<int DIM>
void VoxelGrid<DIM>::flood_from_base_cell(VoxelGrid<DIM>::Mask& mask) {
    Vector_f p = mask.base_coordinates();
    assert(this->is_inside(p));
    const Vector_i seed_idx = this->coordinate_to_index(p);
    if (mask(seed_idx)) return;

    mask(seed_idx) = true;
    std::queue<Vector_i> Q;
    Q.push(seed_idx);
    while (!Q.empty()) {
        const Vector_i idx = Q.front();
        Q.pop();
        assert(mask(idx));

        for (size_t i=0; i<DIM; i++) {
            Vector_i prev(idx); prev[i] -= 1;
            Vector_i next(idx); next[i] += 1;
            if (this->is_valid_index(prev) && !mask(prev)) {
                mask(prev) = true;
                Q.push(prev);
            }
            if (this->is_valid_index(next) && !mask(next)) {
                mask(next) = true;
                Q.push(next);
            }
        }
    }
}


