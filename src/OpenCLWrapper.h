#pragma once
#include <string>
#include <tr1/memory>
extern "C" {
#include "cl-helper.h"
}

class OpenCLWrapper {
    public:
        OpenCLWrapper(bool profile=false);
        virtual ~OpenCLWrapper();

    protected:
        void init_platform();
        void init_device(bool use_cpu);
        void init_context();
        void init_queue();
        void init_program(const std::string& prog);
        void init_program_from_file(const std::string& kernel_file);
        void build_program();

        void init_kernel(const std::string& kernel_name);
        void execute_kernel(size_t num_work_items);

        size_t get_kernel_work_group_size() const;
        size_t get_num_work_groups(size_t total_num_work_items) const;

    protected:
        // Buffer creation methods
        typedef std::tr1::shared_ptr<float> FloatArray;
        typedef std::tr1::shared_ptr<int> IntArray;

        cl_mem create_buffer(size_t num_bytes, void* raw_data=NULL);
        cl_mem create_empty_float_buffer(size_t num_entries) {
            return create_buffer(sizeof(float) * num_entries);
        }

        template<typename T>
        cl_mem create_float_buffer(size_t num_entries, const T* data) {
            FloatArray raw_data = FloatArray(new float[num_entries]);
            std::copy(data, data+num_entries, raw_data.get());
            return create_buffer(sizeof(float)*num_entries, raw_data.get());
        }

        // Overload to handle float array
        cl_mem create_float_buffer(size_t num_entries, const float* data) {
            return create_buffer(sizeof(float)*num_entries,
                    const_cast<float*>(data));
        }

        template<typename T>
        cl_mem create_int_buffer(size_t num_entries, const T* data) {
            IntArray raw_data = IntArray(new int[num_entries]);
            std::copy(data, data+num_entries, raw_data.get());
            return create_buffer(sizeof(int)*num_entries, raw_data.get());
        }

        // Overload to handle int array
        cl_mem create_int_buffer(size_t num_entries, const int* data) {
            return create_buffer(sizeof(int)*num_entries,
                    const_cast<int*>(data));
        }

    protected:
        // Host device data transfer
        void read_from_buffer(cl_mem buf,
                size_t offset, size_t num_bytes, void* data);
        void write_to_buffer(cl_mem buf,
                size_t offset, size_t num_bytes, void* data);

        void attach_profile_callback(cl_event& event);



    private:
        cl_mem allocate_memory_on_device(size_t num_bytes) const;
        std::string read_file(const std::string& filename);

    protected:
        cl_platform_id m_platform;
        cl_device_id   m_device;
        cl_context     m_context;
        cl_program     m_program;
        cl_kernel      m_kernel;
        cl_command_queue m_queue;

        bool           m_profile;
};
