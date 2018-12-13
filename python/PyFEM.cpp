#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Assembler/Materials/Material.h>
#include <Assembler/FEAssembler.h>

namespace py = pybind11;
using namespace PyMesh;

void init_FEM(py::module& m) {
    py::class_<Material, std::shared_ptr<Material> >(m, "Material")
        .def_static("create", &Material::create)
        .def_static("create_isotropic", &Material::create_isotropic)
        .def_static("create_orthotropic", &Material::create_orthotropic)
        .def_static("create_periodic", &Material::create_periodic)
        .def_static("create_element_wise_isotropic", &Material::create_element_wise_isotropic)
        .def_static("create_element_wise_orthotropic", &Material::create_element_wise_orthotropic)
        .def_static("create_element_wise_symmetric", &Material::create_element_wise_symmetric)
        .def("get_material_tensor", &Material::get_material_tensor)
        .def("strain_to_stress", &Material::strain_to_stress)
        .def("get_density", py::overload_cast<>(&Material::get_density, py::const_), "Get denisity (uniform material only).")
        .def("get_density", py::overload_cast<VectorF>(&Material::get_density, py::const_), "Get density at specific point.")
        .def("get_dim", &Material::get_dim)
        .def("update", &Material::update) ;

    py::class_<FEAssembler, std::shared_ptr<FEAssembler> >(m, "FEAssembler")
        .def_static("create", &FEAssembler::create)
        .def_static("create_from_name", &FEAssembler::create_from_name)
        .def("assemble", &FEAssembler::assemble)
        .def("set_material", &FEAssembler::set_material);
}
