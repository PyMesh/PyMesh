#pragma once
#include <numeric>
#include <string>
#include <vector>
#include <tr1/memory>
#include <Environment.h>
#include <OpenCLWrapper.h>

class OpenCLRaceConditionTest : public ::testing::Test, public OpenCLWrapper {
    protected:
        typedef std::vector<float> FloatVector;
        typedef std::tr1::shared_ptr<float> FloatPtr;
        virtual void SetUp () {
            std::string root_dir = Environment::get_required("PYMESH_PATH");
            std::string kernel_file = root_dir +
                "/tests/learning_tests/opencl_kernels/race_condition.cl";

            init_platform();
            init_device(false);
            init_context();
            init_queue();
            init_program_from_file(kernel_file);
            build_program();
            init_kernels();
        }

        virtual void TearDown() {
        }

        void prepare_kernel_under_race_condition() {
            const size_t work_group_size = get_max_work_group_size();
            compute_num_work_items(work_group_size);
            FloatVector data = generate_data();
            m_input = create_float_buffer(m_num_work_items, data.data());
            m_result = create_empty_float_buffer(1);

            SET_2_KERNEL_ARGS(m_kernel, m_input, m_result);
        }

        /**
         * Due to race condition, the output sum is uncertain.
         * So just record the result and pass the test.
         */
        void validate_results_under_race_condition() {
            FloatPtr result(new float);
            read_from_buffer(m_result, 0, sizeof(float), result.get());
            RecordProperty("bad sum", *result);
            SUCCEED();
        }

        void prepare_kernel() {
            const cl_uint data_size = get_data_size();
            m_work_group_size = get_max_work_group_size();
            compute_num_work_items(m_work_group_size);
            FloatVector data = generate_padded_data();
            m_input = create_float_buffer(data_size, data.data());
            m_result = create_empty_float_buffer(m_num_work_groups);
            CALL_CL_GUARDED(clSetKernelArg, (m_kernel, 0, sizeof(cl_mem), &m_input));
            CALL_CL_GUARDED(clSetKernelArg, (m_kernel, 1, sizeof(float)*m_work_group_size, NULL));
            CALL_CL_GUARDED(clSetKernelArg, (m_kernel, 2, sizeof(cl_uint), &data_size));
            CALL_CL_GUARDED(clSetKernelArg, (m_kernel, 3, sizeof(cl_mem), &m_result));
        }

        void validate_results() {
            FloatVector result(m_num_work_groups);
            read_from_buffer(m_result, 0, sizeof(float)*m_num_work_groups, result.data());
            float sum = std::accumulate(result.begin(), result.end(), 0);
            float expected_result = (0+1023) * 1024 / 2;
            ASSERT_FLOAT_EQ(expected_result, sum);
        }

        FloatVector generate_data() {
            const size_t data_size = get_data_size();
            FloatVector data(data_size);
            for (size_t i=0; i<data_size; i++) {
                data[i] = i;
            }
            return data;
        }

        FloatVector generate_padded_data() {
            const size_t data_size = get_data_size();
            FloatVector data(m_num_work_items);
            for (size_t i=0; i<data_size; i++) {
                data[i] = i;
            }
            for (size_t i=data_size; i<m_num_work_items; i++) {
                data[i] = 0;
            }
            return data;
        }

        size_t get_data_size() {
            return 1024;
        }

        size_t compute_num_work_items(size_t work_group_size) {
            const size_t data_size = get_data_size();
            m_num_work_groups = data_size / work_group_size;
            if (data_size % work_group_size > 0)
                m_num_work_groups ++;
            m_num_work_items = m_num_work_groups * work_group_size;
        }

    protected:
        cl_mem m_input;
        cl_mem m_result;
        size_t m_num_work_items;
        size_t m_num_work_groups;
        size_t m_work_group_size;
};

TEST_F(OpenCLRaceConditionTest, RaceCondition) {
    set_kernel("sum_race");
    prepare_kernel_under_race_condition();
    execute_kernel(1, &m_num_work_items, NULL);
    validate_results_under_race_condition();
}

TEST_F(OpenCLRaceConditionTest, NonRaceCondition) {
    set_kernel("sum");
    prepare_kernel();
    execute_kernel(1, &m_num_work_items, &m_work_group_size);
    validate_results();
}
