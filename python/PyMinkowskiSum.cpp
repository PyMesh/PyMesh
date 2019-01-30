#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#if WITH_IGL_AND_CGAL
#include <IGL/MinkowskiSum.h>
#endif

namespace py = pybind11;

void init_MinkowskiSum(py::module& m) {
#if WITH_IGL_AND_CGAL
    using namespace PyMesh;
    py::class_<MinkowskiSum, std::shared_ptr<MinkowskiSum> >(m, "MinkowskiSum")
        .def_static("create", &MinkowskiSum::create)
        .def_static("create_raw", &MinkowskiSum::create_raw)
        .def("run", &MinkowskiSum::run)
        .def("get_vertices", &MinkowskiSum::get_vertices)
        .def("get_faces", &MinkowskiSum::get_faces);
#endif
}
