#pragma once
#include <string>
#include <tr1/memory>
#include <Environment.h>
#include <OpenCLWrapper.h>

class OpenCLVectorOps: public ::testing::Test, public OpenCLWrapper {
    protected:
        virtual void SetUp() {
            std::string root_dir = Environment::get_required("PYMESH_PATH");
            std::string kernel_file = root_dir + "/tests/learning_tests/opencl_kernels/array_mult_scalar.cl";

            init_platform();
            init_device(false);
            init_context();
            init_queue();
            init_program_from_file(kernel_file);
            build_program();
            init_kernels();

            //cat_program();
        }

        virtual void TearDown() {
        }

        void prepare_kernel(size_t vector_width) {
            compute_num_work_items(vector_width);
            FloatArray data = generate_data(vector_width);
            m_input = create_float_buffer(m_num_work_items * vector_width, data.data());
            m_output = create_zero_float_buffer(m_num_work_items * vector_width);

            SET_2_KERNEL_ARGS(m_kernel, m_input, m_output);
        }

        void validate_results(size_t vector_width) {
            const size_t data_size = get_data_size();
            FloatArray result(m_num_work_items * vector_width);
            read_from_buffer(m_output, 0, sizeof(float) * m_num_work_items * vector_width, result.data());

            for (size_t i=0; i<data_size; i++) {
                float x = i;
                float& y = result[i];
                ASSERT_FLOAT_EQ(x*2, y);
            }
        }

        void compute_num_work_items(size_t vector_width) {
            const size_t data_size = get_data_size();
            size_t work_group_size = get_max_work_group_size();
            size_t num_items = data_size / vector_width;
            if (data_size % vector_width > 0)
                num_items++;
            size_t num_work_groups = num_items / work_group_size;
            if (num_items % work_group_size > 0)
                num_work_groups++;
            m_num_work_items = work_group_size * num_work_groups;
        }

        FloatArray generate_data(size_t vector_width=1) {
            const size_t data_size = get_data_size();
            FloatArray data(m_num_work_items * vector_width);
            for (size_t i=0; i<data_size; i++) {
                data[i] = i;
            }
            return data;
        }

        size_t get_data_size() const {
            return 1024;
        }

    protected:
        size_t m_num_work_items;
        cl_mem m_input;
        cl_mem m_output;
};

TEST_F(OpenCLVectorOps, MultBy2) {
    set_kernel("multby2");
    prepare_kernel(1);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(1);
}

TEST_F(OpenCLVectorOps, MultBy2v2) {
    set_kernel("multby2_v2");
    prepare_kernel(2);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(2);
}

//TEST_F(OpenCLVectorOps, DisableLargeKernel) {
//    set_kernel("multby2_v3");
//    prepare_kernel(8);
//    execute_kernel(1, &m_num_work_items, NULL);
//    validate_results(8);
//}
