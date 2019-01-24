#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <SkeletonExtraction/SkeletonEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_SkeletonExtraction(py::module& m) {
    py::class_<SkeletonEngine, std::shared_ptr<SkeletonEngine> >(
            m, "SkeletonEngine")
        .def_static("create", &SkeletonEngine::create)
        .def_static("supports", &SkeletonEngine::supports)
        .def_property_readonly_static("available_engines",
                [](py::object){
                return SkeletonEngine::get_available_engines();})
        .def("run", &SkeletonEngine::run)
        .def("get_edges", &SkeletonEngine::get_edges);
}
