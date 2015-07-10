import unittest
from pymesh.wires import Inflator
from pymesh.wires import Parameters
from pymesh.wires import Tiler
from pymesh.wires import WireNetwork
from WireTestCase import WireTestCase

import numpy as np
from numpy.linalg import norm

class InflatorTest(WireTestCase):
    def test_simple(self):
        wire_network = self.load_wires("brick5.wire");
        wire_network.scale(5);
        inflator = Inflator(wire_network);
        inflator.inflate(0.5);
        mesh = inflator.mesh;

        self.assertLess(0, mesh.num_vertices);
        self.assertLess(0, mesh.num_faces);
        self.assertTrue(mesh.is_closed());

    def test_tiled(self):
        wire_network = self.load_wires("brick5.wire");
        params = Parameters(wire_network, 0.1);
        tiler = Tiler();
        tiler.set_base_pattern(wire_network);
        tiler.tile_with_guide_bbox(
                np.zeros(3), np.ones(3), np.ones(3)*2, params);
        tiled_wire_network = tiler.wire_network;

        inflator = Inflator(tiled_wire_network);
        inflator.inflate(
                tiled_wire_network.get_attribute("thickness").ravel());
        mesh = inflator.mesh;

        self.assertLess(0, mesh.num_vertices);
        self.assertLess(0, mesh.num_faces);
        self.assertTrue(mesh.is_closed());

    def test_periodic(self):
        wire_network = self.load_wires("pattern1065.wire");
        wire_network.scale(5);

        params = Parameters(wire_network, 0.5);
        params.load_dof_file(self.find_file("pattern1065.dof"));

        inflator = Inflator(wire_network);
        inflator.subdivide_order = 0;
        inflator.inflate_periodic(params);
        mesh = inflator.mesh;

        self.assertLess(0, mesh.num_vertices);
        self.assertLess(0, mesh.num_faces);
        self.assertTrue(mesh.is_closed());

