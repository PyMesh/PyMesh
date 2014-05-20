#pragma once
#include <cmath>
#include <string>

#include <Core/Exception.h>
#include <OpenCL/OpenCLWrapper.h>

class OpenCLArctan2Test : public ::testing::Test, public OpenCLWrapper {
    protected:
        virtual void SetUp() {
            m_kernel_path = KERNEL_DIR;
            init_platform();
            init_device(true);
            init_context();
            init_queue();
        }

        virtual void TearDown() {
            release_resources();
        }

        void load_program(const std::string& kernel_file) {
            init_program_from_file(m_kernel_path + kernel_file);
            build_program();
            init_kernels();
        }

        void set_vector_width(size_t vector_width) {
            m_vector_width = vector_width;
        }

        void test_kernel() {
            compute_work_partition();
            prepare_kernel();
            run_kernel();
            validate_results();
        }

    private:
        void compute_work_partition() {
            size_t data_size = get_data_size();
            m_num_work_items = (data_size / m_vector_width) * m_vector_width;
        }

        void prepare_kernel() {
            FloatArray data = generate_data();
            const size_t padded_size = m_num_work_items * m_vector_width;
            m_input_1 = create_float_buffer(padded_size, data.data());
            //m_input_2 = create_float_buffer(padded_size, data.data());
            m_input_2 = create_zero_float_buffer(padded_size);
            m_output = create_zero_float_buffer(padded_size);

            SET_3_KERNEL_ARGS(m_kernel, m_input_1, m_input_2, m_output);
        }

        void run_kernel() {
            execute_kernel(1, &m_num_work_items, NULL);
        }

        void validate_results() {
            const size_t data_size = get_data_size();
            FloatArray result(data_size);
            read_from_buffer(m_output, 0, sizeof(float) * data_size, result.data());

            for (size_t i=0; i<data_size; i++) {
                ASSERT_FLOAT_EQ(atan2(i*2, 0), result[i]);
            }
        }

        FloatArray generate_data() {
            const size_t data_size = get_data_size();
            const size_t padded_size = m_num_work_items * m_vector_width;
            FloatArray data(padded_size);
            for (size_t i=0; i<data_size; i++) {
                data[i] = i;
            }
            return data;
        }

        size_t get_data_size() const {
            return 1024*1024;
        }

    private:
        std::string m_kernel_path;
        size_t m_num_work_items;
        size_t m_vector_width;
        cl_mem m_input_1;
        cl_mem m_input_2;
        cl_mem m_output;
};

TEST_F(OpenCLArctan2Test, ScalarArray) {
    load_program("arctan2_v1.cl");
    set_kernel("arctan2_v1");
    set_vector_width(1);
    test_kernel();
}

TEST_F(OpenCLArctan2Test, VectorArray) {
    load_program("arctan2_v2.cl");
    set_kernel("arctan2_v2");
    set_vector_width(8);
    test_kernel();
}
