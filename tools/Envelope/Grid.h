/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <functional>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

template<int DIM, typename T>
class Grid {
    public:
        typedef Eigen::Matrix<Float, DIM, 1> Vector_f;
        typedef Eigen::Matrix<int, DIM, 1> Vector_i;
        typedef std::vector<T> ArrayType;
        typedef typename ArrayType::reference reference;
        typedef typename ArrayType::const_reference const_reference;
        typedef typename ArrayType::iterator iterator;
        typedef typename ArrayType::const_iterator const_iterator;

        Grid(const Float cell_size) {
            m_cell_size = Vector_f::Ones() * cell_size;
        }
        virtual ~Grid() {}

        void initialize(
                const Vector_i& size, 
                const Vector_f& base_coord) {
            m_grid_size = size;
            m_grid_base_coord = base_coord;
            m_grid = ArrayType(m_grid_size.prod(), 0);
        }

        const Vector_i& size() const { return m_grid_size; }
        const Vector_f& base_coordinates() const { return m_grid_base_coord; }
        const Vector_f& cell_size() const { return m_cell_size; }

        bool is_valid_index(const Vector_i& index) const {
            return ((index.array()>=0).all() && (index.array()<m_grid_size.array()).all());
        }

        bool is_inside(const Vector_f& coordinates) const {
            return is_valid_index(coordinate_to_index(coordinates));
        }

        virtual reference operator()(const Vector_i& index) {
            assert(is_valid_index(index));
            size_t linear_index = linearize_index(index);
            return m_grid[linear_index];
        }

        virtual const_reference operator()(const Vector_i& index) const {
            assert(is_valid_index(index));
            size_t linear_index = linearize_index(index);
            return m_grid[linear_index];
        }

        virtual reference lookup(const Vector_f& coordinates) {
            return this->operator()(coordinate_to_index(coordinates));
        }

        virtual const_reference lookup(const Vector_f& coordinates) const {
            return this->operator()(coordinate_to_index(coordinates));
        }

    public:
        iterator begin() { return m_grid.begin(); }
        const_iterator begin() const { return m_grid.begin(); }
        iterator end() { return m_grid.end(); }
        const_iterator end() const { return m_grid.end(); }

    protected:
        virtual size_t linearize_index(const Vector_i& index) const {
            size_t linear_index = 0;
            for (size_t i=0; i<DIM; i++) {
                linear_index *= m_grid_size[i];
                linear_index += index[i];
            }
            return linear_index;
        }

        virtual Vector_i coordinate_to_index(
                const Vector_f& coordinates) const {
            return (coordinates - m_grid_base_coord)
                .cwiseQuotient(m_cell_size)
                .unaryExpr(std::ptr_fun<Float, Float>(std::round))
                .template cast<int>();
        }

        virtual Vector_f get_cell_center(size_t linear_index) const {
            Vector_f index;
            for (size_t i=DIM-1; i>0; i--) {
                index[i] = linear_index % m_grid_size[i];
                linear_index /= m_grid_size[i];
            }
            index[0] = linear_index;
            assert(index[0] < m_grid_size[0]);

            return m_grid_base_coord + m_cell_size.cwiseProduct(index);
        }

    protected:
        ArrayType m_grid;
        Vector_i m_grid_size;
        Vector_f m_grid_base_coord;
        Vector_f m_cell_size;
};

}
