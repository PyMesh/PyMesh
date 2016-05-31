/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <fstream>
#include <string>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class MshSaver {
    public:
        MshSaver(const std::string& filename, bool binary=true);
        ~MshSaver();

    public:
        // Only these element types are supported.
        enum ElementType {
            TRI = 2,
            QUAD = 3,
            TET = 4,
            HEX = 5
        };

    public:
        void save_mesh(const VectorF& nodes, const VectorI& elements,
                size_t dim, ElementType type);
        void save_header();
        void save_nodes(const VectorF& nodes);
        void save_elements(const VectorI& elements, ElementType type);
        void save_scalar_field(const std::string& fieldname, const VectorF& field);
        void save_vector_field(const std::string& fieldname, const VectorF& field);
        void save_elem_scalar_field(const std::string& fieldname, const VectorF& field);
        void save_elem_vector_field(const std::string& fieldname, const VectorF& field);
        void save_elem_tensor_field(const std::string& fieldname, const VectorF& field);

    public:
        enum ErrorCode {
            INVALID_FORMAT,
            NOT_IMPLEMENTED
        };

    private:
        bool m_binary;
        size_t m_num_nodes;
        size_t m_num_elements;
        size_t m_dim;

        std::ofstream fout;
};

}
