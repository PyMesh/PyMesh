#include "WindingNumberWithOpenCL2.h"
#include <iostream>
#include <Environment.h>
#include <Exception.h>

WindingNumberWithOpenCL2::WindingNumberWithOpenCL2() : OpenCLWrapper(true) {
    std::string proj_path = Environment::get_required("PYMESH_PATH");
    m_kernel_file = proj_path +
        "/tools/windingnumber/opencl_kernels/winding_number.cl";
}

void WindingNumberWithOpenCL2::init(const MatrixFr& V, const MatrixIr& F) {
    init_platform();
    init_device(false);
    init_context();
    init_queue();
    init_program_from_file(m_kernel_file);
    build_program();
    init_kernels();
    set_kernel("compute_solid_angles");

    if (!is_row_major(V))
        throw RuntimeError("Vertex matrix must be row major!");
    if (!is_row_major(F))
        throw RuntimeError("Face matrix must be row major!");
    m_vertex_buf = create_float_buffer(V.size(), V.data());
    m_face_buf   = create_int_buffer  (F.size(), F.data());
    m_num_vertices = V.rows();
    m_num_faces = F.rows();
}

VectorF WindingNumberWithOpenCL2::compute(const MatrixFr& P) {
    /*
    if (!is_row_major(P))
        throw RuntimeError("Point matrix must be row major!");
    size_t num_pts = P.rows();
    size_t num_angles = num_pts * m_num_faces;

    size_t work_group_size = get_kernel_work_group_size();
    if (work_group_size > num_pts) {
        // This should be rare.
        work_group_size = num_pts;
        set_kernel_work_group_size(work_group_size);
    }
    size_t num_work_groups = get_num_work_groups(num_angles);
    size_t num_work_items = num_work_groups * work_group_size;
    std::cout << "work group size: " << work_group_size << std::endl;
    std::cout << "num work groups: " << num_work_groups << std::endl;

    cl_mem point_buffer = create_float_buffer(num_pts * 3, P.data());
    cl_mem wind_num_buffer = create_empty_float_buffer(num_pts);
    
    SET_7_KERNEL_ARGS(m_kernel, 
            m_num_vertices, m_vertex_buf,
            m_num_faces, m_face_buf,
            num_pts, point_buffer, wind_num_buffer);
    execute_kernel(num_work_items);

    FloatArray wind_num_array(num_pts);
    read_from_buffer(wind_num_buffer, 0, num_pts * sizeof(float),
            wind_num_array.data());
    VectorF result(num_pts);
    std::copy(wind_num_array.begin(), wind_num_array.begin()+num_pts, result.data());

    return result;
    */
}

size_t WindingNumberWithOpenCL2::compute_work_load_partition(
        size_t num_items, size_t& dim,
        size_t* global_size, size_t* local_size) {
    dim = 1;
    size_t base_size = get_preferred_work_group_size_multiple();
    size_t multiples = num_items / base_size;
    if (num_items % base_size > 0) {
        multiples ++;
    }
    global_size[0] = base_size * multiples;
    local_size[0] = base_size;
    return global_size[0];
}
