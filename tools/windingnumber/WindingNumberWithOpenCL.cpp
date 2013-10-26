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
    init_kernel("compute_winding_number");

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
    size_t work_group_size = get_kernel_work_group_size();
    size_t num_work_groups = get_num_work_groups(num_pts);
    size_t num_work_items = num_work_groups * work_group_size;
    std::cout << "work group size: " << work_group_size << std::endl;
    std::cout << "num work groups: " << num_work_groups << std::endl;

    FloatArray point_array = pad_floats(num_work_items * 3, P.size(), P.data());
    FloatArray wind_num_array = pad_floats(num_work_items, 0, NULL);

    cl_mem point_buffer = create_float_buffer(
            num_work_items * 3, point_array.data());
    cl_mem wind_num_buffer = create_empty_float_buffer(num_work_items);
    
    SET_7_KERNEL_ARGS(m_kernel, 
            m_num_vertices, m_vertex_buf,
            m_num_faces, m_face_buf,
            num_pts, point_buffer, wind_num_buffer);
    execute_kernel(num_work_items);

    read_from_buffer(wind_num_buffer, 0, num_work_items * sizeof(float),
            wind_num_array.data());
    VectorF result(num_pts);
    std::copy(wind_num_array.begin(), wind_num_array.begin()+num_pts, result.data());

    return result;
}

WindingNumberWithOpenCL::FloatArray WindingNumberWithOpenCL::pad_floats(
        size_t desired_size, size_t num_entries, const Float* data) {
    FloatArray array(desired_size);
    if (num_entries > 0) {
        std::copy(data, data + num_entries, array.begin());
    }
    std::fill(array.begin()+num_entries, array.end(), 0);
    return array;
}

