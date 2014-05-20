#pragma once
#include <cmath>
#include <string>
#include <Misc/Environment.h>
#include <OpenCL/OpenCLWrapper.h>

class OpenCLVectorTest : public ::testing::Test, public OpenCLWrapper {
    protected:
        virtual void SetUp() {
            std::string kernel_dir = KERNEL_DIR;
            std::string kernel_file = kernel_dir + "square.cl";

            init_platform();
            init_device(true);
            init_context();
            init_queue();
            init_program_from_file(kernel_file);
            build_program();
            init_kernels();
        }

        virtual void TearDown() {
            release_resources();
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
                ASSERT_FLOAT_EQ(x*x, y);
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
            return 1024 * 1024;
        }

    protected:
        size_t m_num_work_items;
        cl_mem m_input;
        cl_mem m_output;
};

TEST_F(OpenCLVectorTest, ScalarArrayTest) {
    set_kernel("square");
    prepare_kernel(1);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(1);
}

TEST_F(OpenCLVectorTest, Vector2ArrayTest) {
    set_kernel("square_vec2");
    prepare_kernel(2);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(2);
}

TEST_F(OpenCLVectorTest, Vector4ArrayTest) {
    set_kernel("square_vec4");
    prepare_kernel(4);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(4);
}

TEST_F(OpenCLVectorTest, Vector8ArrayTest) {
    set_kernel("square_vec8");
    prepare_kernel(8);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(8);
}

TEST_F(OpenCLVectorTest, Vector16ArrayTest) {
    set_kernel("square_vec16");
    prepare_kernel(16);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(16);
}

// This test is designed to fail.
TEST_F(OpenCLVectorTest, Vector3ArrayTest) {
    set_kernel("square_vec3");
    prepare_kernel(3);
    execute_kernel(1, &m_num_work_items, NULL);
    float r[8];
    read_from_buffer(m_output, 0, sizeof(float)*8, &r);
    ASSERT_FLOAT_EQ(0, r[0]);
    ASSERT_FLOAT_EQ(1, r[1]);
    ASSERT_FLOAT_EQ(4, r[2]);
    if (fabs(r[3] - 9.0) < 1e-3) {
        std::cout << "float3 is memory aligned" << std::endl;
    } else {
        std::cout << "float3 is NOT memory aligned" << std::endl;
    }
}

TEST_F(OpenCLVectorTest, Vector3ArrayTest2) {
    set_kernel("square_vec3_explicit");
    prepare_kernel(3);
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results(3);
}

