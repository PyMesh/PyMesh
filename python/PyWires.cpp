#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Parameters/ParameterCommon.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Tiler/WireTiler.h>
#include <Wires/Inflator/WireProfile.h>
#include <Wires/Inflator/InflatorEngine.h>
#include <Wires/Misc/SymmetryChecker.h>

namespace py = pybind11;
using namespace PyMesh;
using namespace pybind11::literals;

void init_Wires(py::module &m) {
    py::class_<WireNetwork, std::shared_ptr<WireNetwork> >(m, "WireNetwork")
        .def_static("create", &WireNetwork::create)
        .def_static("create_raw", &WireNetwork::create_raw)
        .def_static("create_empty", &WireNetwork::create_empty)
        .def("get_dim", &WireNetwork::get_dim)
        .def("get_num_vertices", &WireNetwork::get_num_vertices)
        .def("get_num_edges", &WireNetwork::get_num_edges)
        .def("get_vertices", &WireNetwork::get_vertices,
                py::return_value_policy::reference_internal)
        .def("get_edges", &WireNetwork::get_edges,
                py::return_value_policy::reference_internal)
        .def("set_vertices", &WireNetwork::set_vertices)
        .def("set_edges", &WireNetwork::set_edges)
        .def("scale", &WireNetwork::scale)
        .def("scale_fit", &WireNetwork::scale_fit)
        .def("translate", &WireNetwork::translate)
        .def("center_at_origin", &WireNetwork::center_at_origin)
        .def("center", &WireNetwork::center)
        .def("get_bbox_min", &WireNetwork::get_bbox_min)
        .def("get_bbox_max", &WireNetwork::get_bbox_max)
        .def("filter_vertices", &WireNetwork::filter_vertices)
        .def("filter_edges", &WireNetwork::filter_edges)
        .def("write_to_file", &WireNetwork::write_to_file)
        .def("has_attribute", &WireNetwork::has_attribute)
        .def("is_vertex_attribute", &WireNetwork::is_vertex_attribute)
        .def("get_attribute", &WireNetwork::get_attribute,
                py::return_value_policy::reference_internal)
        .def("add_attribute", &WireNetwork::add_attribute,
                "name"_a, "vertex_wise"_a=true, "auto_compute"_a=true)
        .def("set_attribute", &WireNetwork::set_attribute)
        .def("get_attribute_names", &WireNetwork::get_attribute_names)
        .def("clear_attributes", &WireNetwork::clear_attributes)
        .def("with_connectivity", &WireNetwork::with_connectivity)
        .def("compute_connectivity", &WireNetwork::compute_connectivity)
        .def("get_vertex_neighbors", &WireNetwork::get_vertex_neighbors)
        .def("is_printable", &WireNetwork::is_printable);

    py::enum_<ParameterCommon::TargetType>(m, "TargetType")
        .value("VERTEX", ParameterCommon::VERTEX)
        .value("EDGE", ParameterCommon::EDGE)
        .export_values();

    py::class_<ParameterManager, std::shared_ptr<ParameterManager> >
        manager(m, "ParameterManager");
    manager.def_static("create", &ParameterManager::create)
        .def_static("create_isotropic", &ParameterManager::create_isotropic)
        .def_static("create_empty_manager",
                &ParameterManager::create_empty_manager)
        .def_static("create_from_setting_file",
                &ParameterManager::create_from_setting_file)
        .def_static("create_from_dof_file",
                &ParameterManager::create_from_dof_file)
        .def("get_wire_network", &ParameterManager::get_wire_network)
        .def("set_wire_network", &ParameterManager::set_wire_network)
        .def("get_default_thickness", &ParameterManager::get_default_thickness)
        .def("set_default_thickness", &ParameterManager::set_default_thickness)
        .def("get_thickness_params", py::overload_cast<>(
                    &ParameterManager::get_thickness_params, py::const_))
        .def("get_offset_params", py::overload_cast<>(
                    &ParameterManager::get_offset_params, py::const_))
        .def("get_num_dofs", &ParameterManager::get_num_dofs)
        .def("get_num_thickness_dofs",
                &ParameterManager::get_num_thickness_dofs)
        .def("get_num_offset_dofs", &ParameterManager::get_num_offset_dofs)
        .def("get_dofs", &ParameterManager::get_dofs)
        .def("set_dofs", &ParameterManager::set_dofs)
        .def("compute_shape_velocity",
                &ParameterManager::compute_shape_velocity)
        .def("compute_wire_gradient", &ParameterManager::compute_wire_gradient)
        .def("get_thickness_dof_map", &ParameterManager::get_thickness_dof_map)
        .def("get_offset_dof_map", &ParameterManager::get_offset_dof_map)
        .def("save_dofs", &ParameterManager::save_dofs)
        .def("load_dofs", &ParameterManager::load_dofs)
        .def("get_formulas", &ParameterManager::get_formulas)
        .def("get_dof_type", &ParameterManager::get_dof_type)
        .def("evaluate_thickness_no_formula",
                &ParameterManager::evaluate_thickness_no_formula)
        .def("evaluate_thickness", &ParameterManager::evaluate_thickness)
        .def("evaluate_offset_no_formula",
                &ParameterManager::evaluate_offset_no_formula)
        .def("evaluate_offset", &ParameterManager::evaluate_offset)
        .def("get_thickness_type", &ParameterManager::get_thickness_type)
        .def("set_thickness_type", &ParameterManager::set_thickness_type)
        .def("add_thickness_parameter",
                &ParameterManager::add_thickness_parameter)
        .def("get_offset_type", &ParameterManager::get_offset_type)
        .def("set_offset_type", &ParameterManager::set_offset_type)
        .def("add_offset_parameter", &ParameterManager::add_offset_parameter)
        .def("add_custom_offset_parameter",
                &ParameterManager::add_custom_offset_parameter)
        .def("add_isotropic_offset_parameter",
                &ParameterManager::add_isotropic_offset_parameter);

    py::enum_<ParameterManager::DofType>(manager, "DofType")
        .value("ISOTROPIC", ParameterManager::ISOTROPIC)
        .value("ORTHOTROPIC", ParameterManager::ORTHOTROPIC)
        .value("UNKNOWN", ParameterManager::UNKNOWN)
        .export_values();

    py::class_<WireTiler, std::shared_ptr<WireTiler> >(m, "WireTiler")
        .def(py::init<WireNetwork::Ptr>())
        .def("with_parameters", &WireTiler::with_parameters)
        .def("tile_with_guide_bbox", &WireTiler::tile_with_guide_bbox)
        .def("tile_with_guide_mesh", &WireTiler::tile_with_guide_mesh)
        .def("tile_with_mixed_patterns", &WireTiler::tile_with_mixed_patterns);

    py::class_<WireProfile, std::shared_ptr<WireProfile> >(m, "WireProfile")
        .def_static("create", &WireProfile::create)
        .def_static("create_isotropic", &WireProfile::create_isotropic)
        .def_static("create_2D", &WireProfile::create_2D)
        .def("initialize", &WireProfile::initialize)
        .def("with_geometry_correction_lookup",
                &WireProfile::with_geometry_correction_lookup)
        .def("place", &WireProfile::place)
        .def("size", &WireProfile::size);

    py::class_<InflatorEngine, std::shared_ptr<InflatorEngine> >
        inflator( m, "InflatorEngine");

    inflator.def_static("create", &InflatorEngine::create)
        .def_static("create_parametric", &InflatorEngine::create_parametric)
        .def_static("create_isotropic_parametric",
                &InflatorEngine::create_isotropic_parametric)
        .def("with_shape_velocities", &InflatorEngine::with_shape_velocities)
        .def("inflate", &InflatorEngine::inflate)
        .def("get_shape_velocities", &InflatorEngine::get_shape_velocities)
        .def("set_uniform_thickness", &InflatorEngine::set_uniform_thickness)
        .def("set_thickness", &InflatorEngine::set_thickness)
        .def("set_thickness_type", &InflatorEngine::set_thickness_type)
        .def("self_intersection_is_fatal",
                &InflatorEngine::self_intersection_is_fatal)
        .def("with_refinement", &InflatorEngine::with_refinement)
        .def("with_geometry_correction_lookup",
                &InflatorEngine::with_geometry_correction_lookup)
        .def("with_rel_geometry_correction",
                &InflatorEngine::with_rel_geometry_correction)
        .def("with_abs_geometry_correction",
                &InflatorEngine::with_abs_geometry_correction)
        .def("set_geometry_correction_cap",
                &InflatorEngine::set_geometry_correction_cap)
        .def("set_geometry_spread_constant",
                &InflatorEngine::set_geometry_spread_constant)
        .def("get_thickness", &InflatorEngine::get_thickness)
        .def("get_vertices", &InflatorEngine::get_vertices)
        .def("get_faces", &InflatorEngine::get_faces)
        .def("get_face_sources", &InflatorEngine::get_face_sources)
        .def("get_thickness_type", &InflatorEngine::get_thickness_type)
        .def("set_profile", &InflatorEngine::set_profile)
        .def("set_aspect_max", &InflatorEngine::set_aspect_max);

    py::enum_<InflatorEngine::ThicknessType>(inflator, "ThicknessType")
        .value("PER_VERTEX", InflatorEngine::PER_VERTEX)
        .value("PER_EDGE", InflatorEngine::PER_EDGE)
        .export_values();

    py::class_<SymmetryChecker, std::shared_ptr<SymmetryChecker> >(
            m, "SymmetryChecker")
        .def(py::init<WireNetwork::Ptr>())
        .def("has_cubic_symmetry", &SymmetryChecker::has_cubic_symmetry)
        .def("update", &SymmetryChecker::update);
}
