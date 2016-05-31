#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>
extern "C" {
#include "cl-helper.h"
}

namespace PyMesh {

class OpenCLWrapper {
    public:
        OpenCLWrapper(bool profile=false);
        virtual ~OpenCLWrapper();

        void profile_tasks(bool v) {
            m_profile = v;
        }

    protected:
        void init_platform();
        void init_device(bool use_cpu);
        void init_context();
        void init_queue();
        void init_program(const std::string& prog);
        void init_program_from_file(const std::string& kernel_file);
        void build_program();

        // Init and exec kernel
        void init_kernels();
        void set_kernel(const std::string& kernel_name);
        void execute_kernel(size_t dim,
                size_t* global_work_size, size_t* local_work_size);

        // Release all resources
        void release_resources();

        // Query kernel info
        size_t get_max_work_group_size() const;
        size_t get_preferred_work_group_size_multiple() const;
        cl_ulong get_kernel_local_mem_size() const;
        cl_ulong get_kernel_private_mem_size() const;

        // Query device info
        cl_ulong get_device_global_mem_size() const;
        cl_ulong get_device_global_cache_size() const;
        cl_uint  get_device_global_cacheline_size() const;
        cl_ulong get_device_const_mem_size() const;
        cl_ulong get_device_local_mem_size() const;

        // Check kernels
        bool excess_usage_of_local_mem() const;

    protected:
        // Buffer creation methods
        typedef std::vector<float> FloatArray;
        typedef std::vector<int>   IntArray;

        cl_mem create_buffer(size_t num_bytes, void* raw_data=NULL);
        cl_mem create_empty_float_buffer(size_t num_entries) {
            return create_buffer(sizeof(float) * num_entries);
        }
        cl_mem create_zero_float_buffer(size_t num_entries) {
            FloatArray raw_data = FloatArray(num_entries);
            std::fill(raw_data.begin(), raw_data.end(), 0);
            return create_buffer(sizeof(float)*num_entries, raw_data.data());
        }

        template<typename T>
        cl_mem create_float_buffer(size_t num_entries, const T* data) {
            FloatArray raw_data = FloatArray(num_entries);
            std::copy(data, data+num_entries, raw_data.begin());
            return create_buffer(sizeof(float)*num_entries, raw_data.data());
        }

        // Overload to handle float array
        cl_mem create_float_buffer(size_t num_entries, const float* data) {
            return create_buffer(sizeof(float)*num_entries,
                    const_cast<float*>(data));
        }

        template<typename T>
        cl_mem create_int_buffer(size_t num_entries, const T* data) {
            IntArray raw_data = IntArray(num_entries);
            std::copy(data, data+num_entries, raw_data.begin());
            return create_buffer(sizeof(int)*num_entries, raw_data.data());
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

    protected:
        // Debugging methods
        void cat_program();


    private:
        cl_mem allocate_memory_on_device(size_t num_bytes) const;
        std::string read_file(const std::string& filename);

    protected:
        typedef std::map<std::string, cl_kernel> KernelMap;
        typedef std::list<cl_mem> MemList;
        cl_platform_id m_platform;
        cl_device_id   m_device;
        cl_context     m_context;
        cl_program     m_program;
        KernelMap      m_kernels;
        cl_kernel      m_kernel;
        cl_command_queue m_queue;
        MemList        m_buffers;

    private:
        bool           m_profile;
        size_t         m_work_group_size;
};

}
