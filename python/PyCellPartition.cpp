#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <IGL/CellPartition.h>

namespace py = pybind11;
using namespace PyMesh;

void init_CellPartition(py::module& m) {
    py::class_<CellPartition, std::shared_ptr<CellPartition> >(m, "CellPartition")
        .def_static("create", &CellPartition::create)
        .def_static("create_raw", &CellPartition::create_raw)
        .def("run", &CellPartition::run)
        .def("get_vertices", &CellPartition::get_vertices)
        .def("get_faces", &CellPartition::get_faces)
        .def("get_source_faces", &CellPartition::get_source_faces)
        .def("get_num_cells", &CellPartition::get_num_cells)
        .def("get_cell_faces", &CellPartition::get_cell_faces)
        .def("get_cells", &CellPartition::get_cells)
        .def("get_num_patches", &CellPartition::get_num_patches)
        .def("get_patches", &CellPartition::get_patches)
        .def("get_winding_number", &CellPartition::get_winding_number);
}
