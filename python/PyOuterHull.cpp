#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <OuterHull/OuterHullEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_OuterHull(py::module& m) {
    py::class_<OuterHullEngine, std::shared_ptr<OuterHullEngine> >(
            m, "OuterHullEngine")
        .def_static("create", &OuterHullEngine::create)
        .def("run", &OuterHullEngine::run)
        .def("set_mesh", &OuterHullEngine::set_mesh)
        .def("get_vertices", &OuterHullEngine::get_vertices)
        .def("get_faces", &OuterHullEngine::get_faces)
        .def("get_outer_hull_layers", &OuterHullEngine::get_outer_hull_layers)
        .def("get_face_is_flipped", &OuterHullEngine::get_face_is_flipped)
        .def("get_ori_face_indices", &OuterHullEngine::get_ori_face_indices);
}
