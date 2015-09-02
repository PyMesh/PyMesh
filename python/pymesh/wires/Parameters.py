import json
import numpy as np
import re
import os.path

from .WireNetwork import WireNetwork
import PyWires

class Parameters(object):
    """
    This class is a thin wrapper around PyWires.ParameterManager class.
    """

    def __init__(self, wire_network, default_thickness=0.5):
        self.wire_network = wire_network;
        if wire_network.dim == 2:
            self.raw_parameters = PyWires.ParameterManager.create(
                    self.wire_network.raw_wires, default_thickness,
                    PyWires.VERTEX);
        else:
            self.raw_parameters = PyWires.ParameterManager.create_empty_manager(
                    self.wire_network.raw_wires, default_thickness);

    def load_default_isotropic_parameters(self, thickness_type=PyWires.VERTEX):
        self.raw_parameters = PyWires.ParameterManager.create_isotropic(
                self.wire_network.raw_wires, self.default_thickness,
                thickness_type);

    def load_default_orthotropic_parameters(self, thickness_type=PyWires.VERTEX):
        self.raw_parameters = PyWires.ParameterManager.create(
                self.wire_network.raw_wires, self.default_thickness,
                thickness_type);

    def load_modifier_file(self, modifier_file):
        modifier_setting = self.__load_json(modifier_file);
        self.load_modifier(modifier_setting);

    def load_modifier(self, setting):
        self.__initialize_orbits(setting.get("orbit_type", "orthotropic"));

        self.raw_parameters = PyWires.ParameterManager.create_empty_manager(
                self.wire_network.raw_wires, self.default_thickness);
        if "thickness" in setting:
            self.__load_thickness_modifier(setting["thickness"]);
        if "vertex_offset" in setting:
            self.__load_offset_modifier(setting["vertex_offset"]);

    def load_dof_file(self, dof_file):
        assert(os.path.exists(dof_file));
        self.raw_parameters = PyWires.ParameterManager.create_from_dof_file(
                self.wire_network.raw_wires, self.default_thickness, dof_file);

    def save_dof_file(self, dof_file):
        self.raw_parameters.save_dofs(dof_file);

    def __initialize_orbits(self, orbit_type):
        self.wire_network.compute_symmetry_orbits();
        if orbit_type == "isotropic":
            vertex_orbit_ids = self.wire_network.get_attribute(
                    "vertex_cubic_symmetry_orbit").ravel().astype(int);
            edge_orbit_ids = self.wire_network.get_attribute(
                    "edge_cubic_symmetry_orbit").ravel().astype(int);
        elif orbit_type == "orthotropic":
            vertex_orbit_ids = self.wire_network.get_attribute(
                    "vertex_symmetry_orbit").ravel().astype(int);
            edge_orbit_ids = self.wire_network.get_attribute(
                    "edge_symmetry_orbit").ravel().astype(int);
        else:
            raise NotImplementedError("Unknown orbit type: {}".format(
                orbit_type));

        self.orbit_type = orbit_type;
        self.vertex_orbits = self.__group_by_value(vertex_orbit_ids);
        self.edge_orbits = self.__group_by_value(edge_orbit_ids);

    def __load_thickness_modifier(self, thickness_setting):
        """ syntax:
        "thickness" : {
            "type": "vertex_orbit" | "edge_orbit",
            "default": #,
            "effective_orbits": [#, #, ...],
            "thickness": [#, #, ...]
        }
        """
        orbit_ids = thickness_setting["effective_orbits"];
        thickness_values = thickness_setting["thickness"];
        self.default_thickness = thickness_setting["default"];
        assert(len(orbit_ids) == len(thickness_values));

        thickness_type = thickness_setting.get("type", "vertex_orbit");
        self.__set_thickness_type(thickness_type);
        if thickness_type == "vertex_orbit":
            orbits = self.vertex_orbits;
        else:
            orbits = self.edge_orbits;

        for orbit_id, data in zip(orbit_ids, thickness_values):
            formula, value = self.__split_formula_and_value(data);
            self.raw_parameters.add_thickness_parameter(
                    orbits[orbit_id], formula, value);

    def __load_offset_modifier(self, offset_setting):
        """ syntax:
        "vertex_offset": {
            "type": "vertex_orbit",
            "effective_orbits": [#, #, ...],
            "offset_percentages": [
                [#, #, #],
                [#, #, #],
                ...
            ]
        }
        """
        if self.orbit_type == "isotropic":
            raise NotImplementedError(
                    "Modifier file format does not support isotropic offset parameters");

        orbits = self.vertex_orbits;
        orbit_ids = offset_setting["effective_orbits"];
        offset_percentages = offset_setting["offset_percentages"];
        assert(len(orbit_ids) == len(offset_percentages));

        for orbit_id, data in zip(orbit_ids, offset_percentages):
            assert(len(data) == self.wire_network.dim);
            for axis, component in enumerate(data):
                formula, value = self.__split_formula_and_value(component);
                self.raw_parameters.add_offset_parameter(
                        orbits[orbit_id], formula, value, axis);

    def __set_thickness_type(self, thickness_type):
        if thickness_type == "vertex_orbit":
            thickness_type = PyWires.VERTEX;
        elif thickness_type == "edge_orbit":
            thickness_type = PyWires.EDGE;
        else:
            raise RuntimeError(
                    "Unsupported thickness type: {}".format(thickness_type));
        self.raw_parameters.set_thickness_type(thickness_type);

    def __split_formula_and_value(self, data):
        formula_matcher = re.compile("{(.*)}");
        if isinstance(data, (str, unicode)):
            try:
                value = float(data);
                formula = ""
            except ValueError:
                value = 0.0;
                result = formula_matcher.match(data);
                assert(result is not None);
                formula = result.group(1);
        elif isinstance(data, (float, int)):
            value = float(data);
            formula = ""
        else:
            raise TypeError("Cannot split formula and value: {}".format(data));
        return str(formula), value;

    def __load_json(self, json_file):
        with open(json_file, 'r') as fin:
            config = json.load(fin);
        return config;

    def __group_by_value(self, entries):
        num_unique_entires = len(np.unique(entries));
        indices = np.arange(len(entries), dtype=int);
        groups = [indices[entries == i] for i in range(num_unique_entires)];
        return groups;

    @property
    def num_dofs(self):
        return self.raw_parameters.get_num_dofs();

    @property
    def num_thickness_dofs(self):
        return self.raw_parameters.get_num_thickness_dofs();

    @property
    def num_offset_dofs(self):
        return self.raw_parameters.get_num_offset_dofs();

    @property
    def dofs(self):
        return self.raw_parameters.get_dofs().ravel();

    @dofs.setter
    def dofs(self, values):
        self.raw_parameters.set_dofs(values);

    @property
    def default_thickness(self):
        return self.raw_parameters.get_default_thickness();

    @default_thickness.setter
    def default_thickness(self, value):
        self.raw_parameters.set_default_thickness(value);

    @property
    def per_vertex_thickness(self):
        return self.raw_parameters.get_thickness_type() == PyWires.VERTEX;

