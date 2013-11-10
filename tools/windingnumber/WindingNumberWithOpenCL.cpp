#include "WindingNumberWithOpenCL.h"
#include <iostream>
#include <Environment.h>
#include <Exception.h>

WindingNumberWithOpenCL::WindingNumberWithOpenCL() : OpenCLWrapper(true) {
    std::string proj_path = Environment::get_required("PYMESH_PATH");
    m_kernel_file = proj_path +
        "/tools/windingnumber/opencl_kernels/winding_number.cl";
}

void WindingNumberWithOpenCL::init(const MatrixFr& V, const MatrixIr& F) {
    init_platform();
    init_device(false);
    init_context();
    init_queue();
    init_program_from_file(m_kernel_file);
    build_program();
    init_kernels();
    set_kernel("compute_winding_number");

    size_t max_work_group_size = get_max_work_group_size();
    cl_ulong local_mem_size = get_kernel_local_mem_size();
    cl_ulong private_mem_size = get_kernel_private_mem_size();
    size_t preferred_multiple = get_preferred_work_group_size_multiple();
    std::cout << "     max work group size: " << max_work_group_size << std::endl;
    std::cout << "   kernel local mem size: " << local_mem_size << std::endl;
    std::cout << " kernel private mem size: " << private_mem_size << std::endl;
    std::cout << "work group size multiple: " << preferred_multiple << std::endl;

    if (!is_row_major(V))
        throw RuntimeError("Vertex matrix must be row major!");
    if (!is_row_major(F))
        throw RuntimeError("Face matrix must be row major!");
    m_vertex_buf = create_float_buffer(V.size(), V.data());
    m_face_buf   = create_int_buffer  (F.size(), F.data());
    m_num_vertices = V.rows();
    m_num_faces = F.rows();
}

VectorF WindingNumberWithOpenCL::compute(const MatrixFr& P) {
    if (!is_row_major(P))
        throw RuntimeError("Point matrix must be row major!");
    size_t num_pts = P.rows();
    size_t dim, global_size[3], local_size[3];
    size_t num_work_items;
    num_work_items = compute_work_load_partition(num_pts, dim, global_size, local_size);

    FloatArray point_array = pad_floats(num_work_items * 3, P.size(), P.data());
    FloatArray wind_num_array = pad_floats(num_work_items, 0, NULL);

    cl_mem point_buffer = create_float_buffer(
            num_work_items * 3, point_array.data());
    cl_mem wind_num_buffer = create_empty_float_buffer(num_work_items);
    
    SET_7_KERNEL_ARGS(m_kernel, 
            m_num_vertices, m_vertex_buf,
            m_num_faces, m_face_buf,
            num_pts, point_buffer, wind_num_buffer);
    execute_kernel(dim, global_size, NULL);

    read_from_buffer(wind_num_buffer, 0, num_work_items * sizeof(float),
            wind_num_array.data());
    VectorF result(num_pts);
    std::copy(wind_num_array.begin(), wind_num_array.begin()+num_pts, result.data());

    return result;
}

size_t WindingNumberWithOpenCL::compute_work_load_partition(
        size_t num_items, size_t& dim,
        size_t* global_size, size_t* local_size) {
    dim = 1;
    size_t base_size = get_preferred_work_group_size_multiple();
    size_t max_size = get_max_work_group_size();
    base_size = (max_size / base_size) * base_size;
    size_t multiples = num_items / base_size;
    if (num_items % base_size > 0) {
        multiples ++;
    }
    global_size[0] = base_size * multiples;
    local_size[0] = base_size;
    return global_size[0];
}

WindingNumberWithOpenCL::FloatArray WindingNumberWithOpenCL::pad_floats(
        size_t desired_size, size_t num_entries, const Float* data) {
    FloatArray array(desired_size);
    if (num_entries > 0) {
        std::copy(data, data + num_entries, array.begin());
    }
    std::fill(array.begin()+num_entries, array.end(), 1);
    return array;
}

