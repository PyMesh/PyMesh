#pragma once

#include <fstream>
#include <string>

#include <Core/EigenTypedef.h>

class MshSaver {
    public:
        MshSaver(const std::string& filename, bool binary=true);
        ~MshSaver();

    public:
        void save_mesh(const VectorF& nodes, const VectorI& elements,
                size_t dim, size_t nodes_per_element);
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
        void save_ascii_mesh(const VectorF& nodes, const VectorI& elements,
                size_t nodes_per_element);
        void save_binary_mesh(const VectorF& nodes, const VectorI& elements,
                size_t nodes_per_element);

    private:
        bool m_binary;
        size_t m_num_nodes;
        size_t m_num_elements;
        size_t m_dim;

        std::ofstream fout;
};
