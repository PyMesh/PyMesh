import unittest
import numpy as np
import os.path

from WireTestCase import WireTestCase
from math import sqrt

class WireNetworkTest(WireTestCase):
    def test_creation(self):
        wire_network = self.get_brick5();
        self.assertEqual(20, wire_network.num_vertices);
        self.assertEqual(30, wire_network.num_edges);

    def test_connectivity(self):
        wire_network = self.get_star_3D();
        self.assertEqual(3, wire_network.dim);
        for i in range(8):
            self.assertEqual([8], wire_network.get_vertex_neighbors(i));
        self.assert_array_equal(
                range(8),
                sorted(wire_network.get_vertex_neighbors(8)));

    def test_total_length(self):
        wire_network = self.get_star_2D();
        self.assertEqual(2, wire_network.dim);
        self.assertAlmostEqual(2*sqrt(2), wire_network.total_wire_length);

    def test_trim(self):
        wire_network = self.get_brick5();
        wire_network.trim();
        self.assertEqual(14, wire_network.num_vertices);
        self.assertEqual(24, wire_network.num_edges);

    def test_2D_wire(self):
        wire_network = self.get_star_2D();
        self.assertEqual(2, wire_network.dim);
        self.assertEqual(5, wire_network.num_vertices);
        self.assertEqual(4, wire_network.num_edges);

        bbox_center = wire_network.bbox_center;
        self.assertListEqual([0.5, 0.5], bbox_center.tolist());

    def test_symmetry_orbits(self):
        wire_network = self.get_brick5();
        wire_network.compute_symmetry_orbits();

        self.assertTrue(wire_network.has_attribute("vertex_symmetry_orbit"));
        self.assertTrue(wire_network.has_attribute("vertex_cubic_symmetry_orbit"));
        self.assertTrue(wire_network.has_attribute("edge_symmetry_orbit"));
        self.assertTrue(wire_network.has_attribute("edge_cubic_symmetry_orbit"));

        vertex_symmetry_orbit = wire_network.get_attribute("vertex_symmetry_orbit");
        self.assertEqual(wire_network.num_vertices, len(vertex_symmetry_orbit));
        self.assertEqual(7, len(np.unique(vertex_symmetry_orbit)));

        vertex_cubic_symmetry_orbit = wire_network.get_attribute(
                "vertex_cubic_symmetry_orbit");
        self.assertEqual(wire_network.num_vertices,
                len(vertex_cubic_symmetry_orbit));
        self.assertEqual(3, len(np.unique(vertex_cubic_symmetry_orbit)));

        edge_symmetry_orbit = wire_network.get_attribute("edge_symmetry_orbit");
        self.assertEqual(wire_network.num_edges, len(edge_symmetry_orbit));
        self.assertEqual(6, len(np.unique(edge_symmetry_orbit)));

        edge_cubic_symmetry_orbit = wire_network.get_attribute(
                "edge_cubic_symmetry_orbit");
        self.assertEqual(wire_network.num_edges,
                len(edge_cubic_symmetry_orbit));
        self.assertEqual(2, len(np.unique(edge_cubic_symmetry_orbit)));

