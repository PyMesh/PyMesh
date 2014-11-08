import unittest
import numpy as np
from WireTest import WireTest

import PyWires

class WireTilerTest(WireTest):
    def test_tile_with_AABB_once(self):
        wires = self.load_wire_file("cube.wire");
        tiler = PyWires.WireTiler(wires);
        tiled_wires = tiler.tile_with_guide_bbox(
                np.zeros(3), np.ones(3) * 10,
                np.ones(3, dtype=int));

        self.assertEqual(wires.get_num_vertices(),
                tiled_wires.get_num_vertices());
        self.assertEqual(wires.get_num_edges(),
                tiled_wires.get_num_edges());
        self.assertListEqual([0, 0, 0],
                tiled_wires.get_bbox_min().ravel().tolist());
        self.assertListEqual([10, 10, 10],
                tiled_wires.get_bbox_max().ravel().tolist());

    def test_tile_with_AABB(self):
        wires = self.load_wire_file("cube.wire");
        tiler = PyWires.WireTiler(wires);
        tiled_wires = tiler.tile_with_guide_bbox(
                np.zeros(3), np.ones(3) * 10,
                np.array([2, 2, 2]));

        self.assertEqual(27, tiled_wires.get_num_vertices());
        self.assertEqual(54, tiled_wires.get_num_edges());

    def test_tile_with_AABB_2D(self):
        wires = self.load_wire_file("square.wire");
        tiler = PyWires.WireTiler(wires);
        tiled_wires = tiler.tile_with_guide_bbox(
                np.zeros(2), np.ones(2), np.array([2, 2]));
        self.assertEqual(9, tiled_wires.get_num_vertices());
        self.assertEqual(12, tiled_wires.get_num_edges());

    def test_tile_with_mesh(self):
        mesh = self.load_mesh("hex.msh");
        wires = self.load_wire_file("star_3D.wire");

        tiler = PyWires.WireTiler(wires);
        tiled_wires = tiler.tile_with_guide_mesh(mesh);

        self.assertEqual(wires.get_num_vertices(),
                tiled_wires.get_num_vertices());
        self.assertEqual(wires.get_num_edges(),
                tiled_wires.get_num_edges());

    def test_tile_with_mesh_2D(self):
        mesh = self.load_mesh("quad.obj");
        wires = self.load_wire_file("star_2D.wire");

        tiler = PyWires.WireTiler(wires);
        tiled_wires = tiler.tile_with_guide_mesh(mesh);

        self.assertEqual(wires.get_num_vertices(),
                tiled_wires.get_num_vertices());
        self.assertEqual(wires.get_num_edges(),
                tiled_wires.get_num_edges());

