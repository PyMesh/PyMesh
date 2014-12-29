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

    def test_mixed_mesh_tiling_2D(self):
        mesh = self.load_mesh("quad_s1.obj");
        wires = [self.load_wire_file("box.wire"),
                self.load_wire_file("cross.wire")];
        mesh.add_attribute("pattern_id");
        mesh.set_attribute("pattern_id", np.array([0, 1, 1, 0]));
        for i in range(5):
            attr_name = "dof_{}".format(i);
            attr_value = np.array([0.1, 0.1, 0.1, 0.1]);
            mesh.add_attribute(attr_name);
            mesh.set_attribute(attr_name, attr_value);

        tiler = PyWires.WireTiler(None);
        tiled_wires = tiler.tile_with_mixed_patterns(wires, mesh, True, True);
        self.assertTrue(tiled_wires.has_attribute("thickness"));
        self.assertTrue(tiled_wires.has_attribute("vertex_offset"));

        self.assertEqual(np.sum(wire.get_num_vertices()*2 for wire in wires) - 4,
                tiled_wires.get_num_vertices());
        self.assertEqual(np.sum(wire.get_num_edges()*2 for wire in wires),
                tiled_wires.get_num_edges());

        thickness = tiled_wires.get_attribute("thickness");
        self.assertAlmostEqual(0.1, np.amin(thickness));
        self.assertAlmostEqual(0.1, np.amax(thickness));

    def test_mixed_mesh_tiling_3D(self):
        mesh = self.load_mesh("hex_s1.msh");
        wires = [self.load_wire_file("brick5.wire"),
                self.load_wire_file("cross_3D.wire")];
        mesh.add_attribute("pattern_id");
        mesh.set_attribute("pattern_id", np.array([0, 1, 1, 0, 1, 0, 0, 1]));
        for i in range(5):
            attr_name = "dof_{}".format(i);
            attr_value = np.ones(8) * 0.1;
            mesh.add_attribute(attr_name);
            mesh.set_attribute(attr_name, attr_value);

        tiler = PyWires.WireTiler(None);
        tiled_wires = tiler.tile_with_mixed_patterns(wires, mesh, True, True);
        self.assertTrue(tiled_wires.has_attribute("thickness"));
        self.assertTrue(tiled_wires.has_attribute("vertex_offset"));

        self.assertEqual(np.sum(wire.get_num_vertices()*4 for wire in wires) - 12,
                tiled_wires.get_num_vertices());
        self.assertEqual(np.sum(wire.get_num_edges()*4 for wire in wires),
                tiled_wires.get_num_edges());

        thickness = tiled_wires.get_attribute("thickness");
        self.assertAlmostEqual(0.1, np.amin(thickness));
        self.assertAlmostEqual(0.1, np.amax(thickness));

