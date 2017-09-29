#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <IGL/MinkowskiSum.h>

namespace py = pybind11;
using namespace PyMesh;

void init_MinkowskiSum(py::module& m) {
    py::class_<MinkowskiSum, std::shared_ptr<MinkowskiSum> >(m, "MinkowskiSum")
        .def_static("create", &MinkowskiSum::create)
        .def_static("create_raw", &MinkowskiSum::create_raw)
        .def("run", &MinkowskiSum::run)
        .def("get_vertices", &MinkowskiSum::get_vertices)
        .def("get_faces", &MinkowskiSum::get_faces);
}
