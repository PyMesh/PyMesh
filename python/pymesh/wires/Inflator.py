import numbers
import numpy as np
import logging

import PyWires
from ..meshio import form_mesh

class Inflator(object):
    def __init__(self, wire_network):
        self.logger = logging.getLogger(__name__);
        self.wire_network = wire_network;

        # Geometry correction.
        self.rel_geometry_correction = None;
        self.abs_geometry_correction = None;
        self.geometry_correction_cap = None;
        self.geometry_spread = None;
        self.geometry_correction_lookup = None;

        # Wire profile
        self.profile = None;

        # Post-subdivision
        self.subdivide_order = 1;
        self.subdivide_method="simple";

    def set_geometry_correction(self,
            rel_geometry_correction=None,
            abs_geometry_correction=None,
            geometry_correction_cap=None,
            geometry_spread=None,
            geometry_correction_lookup=None):
        self.rel_geometry_correction = rel_geometry_correction;
        self.abs_geometry_correction = abs_geometry_correction;
        self.geometry_correction_cap = geometry_correction_cap;
        self.geometry_spread = geometry_spread;
        self.geometry_correction_lookup = geometry_correction_lookup;

    def set_profile(self, N):
        """ Set the cross section shape of each wire to N-gon.
        """
        if self.wire_network.dim != 3:
            raise NotImplementedError("Wire profile only works in 3D.");
        self.profile = PyWires.WireProfile.create_isotropic(N);

    def set_refinement(self, order=1, method="loop"):
        """ Refine the output mesh using subdivision.

        Arguments:
            order: how many times to subdivide.
            mehtod: which subdivision scheme to use.
                    Options are ``loop`` and ``simple``.
        """
        if not isinstance(order, int) or order < 0:
            raise RuntimeError("Invalid subdivision order: {}".format(order));
        if method not in ("loop", "simple"):
            raise NotImplementedError(
                    "Unsupport subdivision method: {}".format(method));

        self.subdivide_order = order;
        self.subdivide_method = method;

    def inflate(self, thickness, per_vertex_thickness=True,
            allow_self_intersection=False):
        wires = self.wire_network.raw_wires;
        inflator = PyWires.InflatorEngine.create("simple", wires);
        if not allow_self_intersection:
            inflator.self_intersection_is_fatal();

        if per_vertex_thickness:
            if isinstance(thickness, numbers.Number):
                thickness = np.ones(self.wire_network.num_vertices) * thickness;
            assert(len(thickness) == self.wire_network.num_vertices);
            inflator.set_thickness_type(PyWires.VERTEX);
        else:
            if isinstance(thickness, numbers.Number):
                thickness = np.ones(self.wire_network.num_edges) * thickness;
            assert(len(thickness) == self.wire_network.num_edges);
            inflator.set_thickness_type(PyWires.EDGE);
        inflator.set_thickness(thickness);

        self.__setup_geometry_correction(inflator);
        self.__setup_subdivision(inflator);
        self.__setup_profile(inflator);

        inflator.inflate();

        self.mesh_vertices = inflator.get_vertices();
        self.mesh_faces = inflator.get_faces();
        self.source_wire_id = inflator.get_face_sources();

    def inflate_periodic(self, parameters):
        wires = self.wire_network.raw_wires;
        assert(parameters.wire_network == self.wire_network);

        symm_checker = PyWires.SymmetryChecker(wires);
        if (symm_checker.has_cubic_symmetry()):
            self.logger.info("Using reflective inflator");
            inflator = PyWires.InflatorEngine.create_isotropic_parametric(
                    wires, parameters.raw_parameters);
        else:
            self.logger.info("Using periodic inflator");
            inflator = PyWires.InflatorEngine.create_parametric(
                    wires, parameters.raw_parameters);

        self.__setup_geometry_correction(inflator);
        self.__setup_subdivision(inflator);
        self.__setup_profile(inflator);

        inflator.inflate();
        self.mesh_vertices = inflator.get_vertices();
        self.mesh_faces = inflator.get_faces();
        self.source_wire_id = inflator.get_face_sources();

        self.wire_network.compute_symmetry_orbits();


    def __setup_geometry_correction(self, inflator):
        if (self.rel_geometry_correction is not None):
            inflator.with_rel_geometry_correction(
                    np.array(self.rel_geometry_correction));
        if (self.abs_geometry_correction is not None):
            inflator.with_abs_geometry_correction(
                    np.array(self.abs_geometry_correction));
        if (self.geometry_correction_cap is not None):
            inflator.set_geometry_correction_cap(self.geometry_correction_cap);
        if (self.geometry_spread is not None):
            inflator.set_geometry_spread_constant(self.geometry_spread);
        if (self.geometry_correction_lookup is not None):
            inflator.with_geometry_correction_lookup(
                    self.geometry_correction_lookup);

    def __setup_subdivision(self, inflator):
        inflator.with_refinement(
                self.subdivide_method, self.subdivide_order);

    def __setup_profile(self, inflator):
        if self.profile is not None:
            inflator.set_profile(self.profile);

    @property
    def mesh(self):
        mesh = form_mesh(self.mesh_vertices, self.mesh_faces);
        mesh.add_attribute("source_wire_id");
        mesh.set_attribute("source_wire_id", self.source_wire_id);

        if self.wire_network.has_attribute("vertex_symmetry_orbit"):
            indices = self.wire_network.get_attribute("vertex_symmetry_orbit").ravel();
            source_id_mask = self.source_wire_id > 0;
            source_index = np.zeros_like(self.source_wire_id);
            source_index[np.logical_not(source_id_mask)] = -1;
            source_index[source_id_mask] =\
                    indices[self.source_wire_id[source_id_mask] - 1];
            mesh.add_attribute("orthotropic_vertex_orbit");
            mesh.set_attribute("orthotropic_vertex_orbit", source_index);

        if self.wire_network.has_attribute("vertex_cubic_symmetry_orbit"):
            indices = self.wire_network.get_attribute("vertex_cubic_symmetry_orbit").ravel();
            source_id_mask = self.source_wire_id > 0;
            source_index = np.zeros_like(self.source_wire_id);
            source_index[np.logical_not(source_id_mask)] = -1;
            source_index[source_id_mask] =\
                    indices[self.source_wire_id[source_id_mask] - 1];
            mesh.add_attribute("isotropic_vertex_orbit");
            mesh.set_attribute("isotropic_vertex_orbit", source_index);

        if self.wire_network.has_attribute("edge_symmetry_orbit"):
            indices = self.wire_network.get_attribute("edge_symmetry_orbit").ravel();
            source_id_mask = self.source_wire_id < 0;
            source_index = np.zeros_like(self.source_wire_id);
            source_index[np.logical_not(source_id_mask)] = -1;
            source_index[source_id_mask] =\
                    indices[-self.source_wire_id[source_id_mask] - 1];
            mesh.add_attribute("orthotropic_edge_orbit");
            mesh.set_attribute("orthotropic_edge_orbit", source_index);

        if self.wire_network.has_attribute("edge_cubic_symmetry_orbit"):
            indices = self.wire_network.get_attribute("edge_cubic_symmetry_orbit").ravel();
            source_id_mask = self.source_wire_id < 0;
            source_index = np.zeros_like(self.source_wire_id);
            source_index[np.logical_not(source_id_mask)] = -1;
            source_index[source_id_mask] =\
                    indices[-self.source_wire_id[source_id_mask] - 1];
            mesh.add_attribute("isotropic_edge_orbit");
            mesh.set_attribute("isotropic_edge_orbit", source_index);

        return mesh;

