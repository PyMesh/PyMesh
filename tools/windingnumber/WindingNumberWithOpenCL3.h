#pragma once
#include <string>
#include <OpenCLWrapper.h>
#include "WindingNumber.h"

class WindingNumberWithOpenCL3 : public WindingNumber, public OpenCLWrapper {
    public:
        WindingNumberWithOpenCL3();
        virtual ~WindingNumberWithOpenCL3() {}

    public:
        virtual void init(const MatrixFr& V, const MatrixIr& F);
        virtual VectorF compute(const MatrixFr& P);

    private:
        FloatArray unpack_vertex_data(const MatrixFr& V, const MatrixIr& F);
        FloatArray align_to_memory(const MatrixFr& P);
        size_t compute_work_load_partition(size_t num_items, size_t& dim,
                size_t* global_size, size_t* local_size);
        FloatArray pad_floats(size_t desired_size, size_t num_entries, const Float* data);

        template<typename Derived>
        bool is_row_major(const Eigen::MatrixBase<Derived>& M) {
            return M.IsRowMajor;
        }

    private:
        std::string m_kernel_file;
        size_t m_num_vertices;
        size_t m_num_faces;
        cl_mem m_vertex_buf;

        const size_t m_vector_width;
};
