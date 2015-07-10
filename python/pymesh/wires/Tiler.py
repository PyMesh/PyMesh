import numpy as np

import PyWires
from WireNetwork import WireNetwork

class Tiler(object):
    def set_base_pattern(self, network):
        network.offset(-network.bbox_center);
        self.pattern = network;
        self.raw_pattern = self.pattern.raw_wires;

    def set_base_patterns(self, networks):
        assert(len(networks) > 0);
        self.patterns = networks;
        self.raw_patterns = [network.raw_wires for network in networks];

    def tile_with_guide_bbox(self, bbox_min, bbox_max, reps, parameters):
        tiler = PyWires.WireTiler(self.raw_pattern);
        tiler.with_parameters(parameters.raw_parameters);
        self.raw_wire_network = tiler.tile_with_guide_bbox(bbox_min, bbox_max, reps);
        self.__apply_vertex_offset();

    def tile_with_guide_mesh(self, mesh, parameters):
        tiler = PyWires.WireTiler(self.raw_pattern);
        tiler.with_parameters(parameters.raw_parameters);
        self.raw_wire_network = tiler.tile_with_guide_mesh(mesh.raw_mesh);
        self.__apply_vertex_offset();

    def tile_with_mixed_patterns(self, mesh,
            per_vertex_thickness=False, isotropic_dofs=True):
        tiler = PyWires.WireTiler(self.raw_patterns[0]);
        self.raw_wire_network = tiler.tile_with_mixed_patterns(
                self.raw_patterns, mesh.raw_mesh,
                per_vertex_thickness, isotropic_dofs);
        self.__apply_vertex_offset();

    def __apply_vertex_offset(self):
        vertices = self.raw_wire_network.get_vertices();
        offset = self.raw_wire_network.get_attribute("vertex_offset");
        self.raw_wire_network.set_vertices(vertices + offset);

    @property
    def wire_network(self):
        tiled_wire_network = WireNetwork();
        tiled_wire_network.load_from_raw(self.raw_wire_network);
        return tiled_wire_network;
