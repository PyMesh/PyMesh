#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Misc/HashGrid.h>

namespace py = pybind11;
using namespace PyMesh;

void init_HashGrid(py::module &m) {
    py::class_<HashGrid, std::shared_ptr<HashGrid> > hash_grid(m, "HashGrid");
    hash_grid.def_static("create",
            [](Float cell_size, size_t dim) {
                return HashGrid::create(cell_size, dim);
            })
        .def("insert", &HashGrid::insert)
        .def("insert_bbox", &HashGrid::insert_bbox)
        .def("insert_triangle", &HashGrid::insert_triangle)
        .def("insert_multiple_triangles", &HashGrid::insert_multiple_triangles)
        .def("insert_batch", &HashGrid::insert_batch)
        .def("insert_multiple", &HashGrid::insert_multiple)
        .def("remove", &HashGrid::remove)
        .def("occupied", &HashGrid::occupied)
        .def("bucket_count", &HashGrid::bucket_count)
        .def("size", &HashGrid::size)
        .def("get_items_near_point", &HashGrid::get_items_near_point)
        .def("get_occupied_cell_centers", &HashGrid::get_occupied_cell_centers);
}
