#include "WindingNumberWithOpenCL3.h"
#include <iostream>
#include <Environment.h>
#include <Exception.h>

WindingNumberWithOpenCL3::WindingNumberWithOpenCL3() : OpenCLWrapper(true), m_vector_width(4) {
    std::string proj_path = Environment::get_required("PYMESH_PATH");
    m_kernel_file = proj_path +
        "/tools/windingnumber/opencl_kernels/winding_number_v4_small.cl";
}

void WindingNumberWithOpenCL3::init(const MatrixFr& V, const MatrixIr& F) {
    init_platform();
    init_device(true);
    init_context();
    init_queue();
    init_program_from_file(m_kernel_file);
    build_program();
    init_kernels();
    set_kernel("compute_winding_number_v4");

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

    FloatArray vertex_data = unpack_vertex_data(V, F);
    m_vertex_buf = create_float_buffer(vertex_data.size(), vertex_data.data());
    m_num_vertices = V.rows();
    m_num_faces = F.rows();
}

VectorF WindingNumberWithOpenCL3::compute(const MatrixFr& P) {
    if (!is_row_major(P))
        throw RuntimeError("Point matrix must be row major!");
    size_t num_pts = P.rows();
    std::cout << "num pts: " << num_pts << std::endl;
    size_t dim, global_size[3], local_size[3];
    size_t num_work_items;
    num_work_items = compute_work_load_partition(num_pts, dim, global_size, local_size);
    std::cout << dim << " " << global_size[0] << " " << local_size[0] << std::endl;
    std::cout << "pts buffer size: " << num_work_items * 4 * m_vector_width << std::endl;
    std::cout << "num faces: " << m_num_faces << std::endl;

    FloatArray pts_array = align_to_memory(P);
    pts_array.resize(num_work_items * 4 * m_vector_width, 0);
    FloatArray wind_num_array = pad_floats(num_work_items * m_vector_width, 0, NULL);

    cl_mem point_buffer = create_float_buffer(
            pts_array.size(), pts_array.data());
    cl_mem wind_num_buffer = create_zero_float_buffer(num_work_items * m_vector_width);

    SET_4_KERNEL_ARGS(m_kernel, 
            m_vertex_buf, m_num_faces,
            point_buffer, wind_num_buffer);
    execute_kernel(dim, global_size, local_size);

    read_from_buffer(wind_num_buffer, 0, num_work_items * sizeof(float) * m_vector_width,
            wind_num_array.data());
    VectorF result(num_pts);
    std::copy(wind_num_array.begin(), wind_num_array.begin()+num_pts, result.data());

    return result;
}

WindingNumberWithOpenCL3::FloatArray WindingNumberWithOpenCL3::unpack_vertex_data(
        const MatrixFr& V, const MatrixIr& F) {
    // each face has 3 vertices, each vertex occupies 4 floats
    const size_t num_faces = F.rows();
    FloatArray vertex_data(num_faces * 3 * 4);
    for (size_t i=0; i<num_faces; i++) {
        const Vector3F& v1 = V.row(F(i, 0));
        const Vector3F& v2 = V.row(F(i, 1));
        const Vector3F& v3 = V.row(F(i, 2));
        vertex_data[i*3*4 + 0] = v1[0];
        vertex_data[i*3*4 + 1] = v1[1];
        vertex_data[i*3*4 + 2] = v1[2];
        vertex_data[i*3*4 + 3] = 0;

        vertex_data[i*3*4 + 4] = v2[0];
        vertex_data[i*3*4 + 5] = v2[1];
        vertex_data[i*3*4 + 6] = v2[2];
        vertex_data[i*3*4 + 7] = 0;

        vertex_data[i*3*4 + 8 ] = v3[0];
        vertex_data[i*3*4 + 9 ] = v3[1];
        vertex_data[i*3*4 + 10] = v3[2];
        vertex_data[i*3*4 + 11] = 0;
    }
    return vertex_data;
}

WindingNumberWithOpenCL3::FloatArray WindingNumberWithOpenCL3::align_to_memory(
        const MatrixFr& P) {
    const size_t num_pts = P.rows();
    FloatArray result(num_pts * 4);
    for (size_t i=0; i<num_pts; i++) {
        result[i*4+0] = P(i, 0);
        result[i*4+1] = P(i, 1);
        result[i*4+2] = P(i, 2);
        result[i*4+3] = 0;
    }
    return result;
}

size_t WindingNumberWithOpenCL3::compute_work_load_partition(
        size_t num_items, size_t& dim,
        size_t* global_size, size_t* local_size) {
    dim = 1;
    const size_t vector_width = m_vector_width;
    size_t base_size = get_preferred_work_group_size_multiple();
    size_t max_size = get_max_work_group_size();
    //base_size = (max_size / base_size) * base_size;
    base_size = 1;

    size_t num_vector_items = num_items / vector_width;
    if (num_items % vector_width > 0)
        num_vector_items ++;
    size_t multiples = num_vector_items / base_size;
    if (num_vector_items % base_size > 0)
        multiples ++;

    global_size[0] = base_size * multiples;
    local_size[0] = base_size;
    return global_size[0];
}

WindingNumberWithOpenCL3::FloatArray WindingNumberWithOpenCL3::pad_floats(
        size_t desired_size, size_t num_entries, const Float* data) {
    FloatArray array(desired_size);
    if (num_entries > 0) {
        std::copy(data, data + num_entries, array.begin());
    }
    std::fill(array.begin()+num_entries, array.end(), 0);
    return array;
}



