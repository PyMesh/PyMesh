import numpy as np
import unittest
from WireTest import WireTest

class WireNetworkTest(WireTest):
    def test_creation(self):
        wires = self.load_wire_file("cube.wire");
        self.assertEqual(3, wires.get_dim());
        self.assertEqual(8, wires.get_num_vertices());
        self.assertEqual(12, wires.get_num_edges());

    def test_brick5(self):
        wires = self.load_wire_file("brick5.wire");
        self.assertEqual(3, wires.get_dim());
        self.assertEqual(20, wires.get_num_vertices());
        self.assertEqual(30, wires.get_num_edges());

    def test_box(self):
        wires = self.load_wire_file("box.wire");
        self.assertEqual(2, wires.get_dim());
        self.assertEqual(12, wires.get_num_vertices());
        self.assertEqual(12, wires.get_num_edges());

    def test_scale(self):
        wires = self.load_wire_file("star_3D.wire");
        ori_bbox_size = wires.get_bbox_max() - wires.get_bbox_min();
        factor = np.array([2, 2, 2]);
        wires.scale(factor);
        cur_bbox_size = wires.get_bbox_max() - wires.get_bbox_min();

        self.assertListEqual((ori_bbox_size * 2).tolist(),
                cur_bbox_size.tolist());

    def test_transform(self):
        wires = self.load_wire_file("diamond.wire");
        wires.center_at_origin();
        self.assertListEqual(
                (wires.get_bbox_min() * -1).tolist(),
                wires.get_bbox_max().tolist());
        wires.scale_fit(np.zeros(3), np.ones(3));
        self.assertListEqual([0, 0, 0],
                wires.get_bbox_min().ravel().tolist());
        self.assertListEqual([1, 1, 1],
                wires.get_bbox_max().ravel().tolist());

    def test_attributes(self):
        wires = self.load_wire_file("diamond.wire");
        num_vertices = wires.get_num_vertices();
        self.assertFalse(wires.has_attribute("test"));
        wires.add_attribute("test", True);
        self.assertTrue(wires.has_attribute("test"));
        wires.set_attribute("test", np.arange(num_vertices));
        attr = wires.get_attribute("test");
        self.assertTupleEqual((num_vertices, 1), attr.shape);
        self.assertListEqual(np.arange(num_vertices).tolist(), attr.ravel().tolist());
        self.assertIn("test", wires.get_attribute_names());

    def test_connectivity(self):
        wires = self.load_wire_file("star_2D.wire");
        self.assertFalse(wires.with_connectivity());
        wires.compute_connectivity();
        self.assertTrue(wires.with_connectivity());
        for edge in wires.get_edges():
            self.assertIn(edge[0],
                    wires.get_vertex_neighbors(int(edge[1])).ravel().tolist());
            self.assertIn(edge[1],
                    wires.get_vertex_neighbors(int(edge[0])).ravel().tolist());


