from pymesh.TestCase import TestCase
from pymesh.meshutils import generate_box_mesh
from pymesh import minkowski_sum, detect_self_intersection

import numpy as np

class MinkowskiSumTest(TestCase):
    def test_simple(self):
        input_mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        path = np.array([ [0, 0, 0], [1, 1, 1] ]);

        output_mesh = minkowski_sum(input_mesh, path);
        self.assertTrue(output_mesh.is_closed());
        self.assertTrue(output_mesh.is_oriented());
        self.assertTrue(output_mesh.num_boundary_edges == 0);

        input_bbox_min, input_bbox_max = input_mesh.bbox;
        output_bbox_min, output_bbox_max = output_mesh.bbox;

        self.assert_array_equal(input_bbox_min, output_bbox_min);
        self.assert_array_equal([1, 1, 1], output_bbox_max - input_bbox_max);

    def test_coplanar(self):
        input_mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        path = np.array([ [0, 0, 0], [1e-12, 0, 0] ]);

        output_mesh = minkowski_sum(input_mesh, path);
        self.assertTrue(output_mesh.is_closed());
        self.assertTrue(output_mesh.is_oriented());
        self.assertTrue(output_mesh.num_boundary_edges == 0);

        input_bbox_min, input_bbox_max = input_mesh.bbox;
        output_bbox_min, output_bbox_max = output_mesh.bbox;

        self.assert_array_equal(input_bbox_min, output_bbox_min);
        self.assert_array_almost_equal([1e-12, 0, 0],
                output_bbox_max - input_bbox_max);

    def test_near_coplanar(self):
        input_mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        path = np.array([ [0, 0, 0], [100, 1e-3, 0] ]);

        output_mesh = minkowski_sum(input_mesh, path);
        self.assertTrue(output_mesh.is_closed());
        self.assertTrue(output_mesh.is_oriented());
        self.assertTrue(output_mesh.num_boundary_edges == 0);

        input_bbox_min, input_bbox_max = input_mesh.bbox;
        output_bbox_min, output_bbox_max = output_mesh.bbox;

        self.assert_array_equal(input_bbox_min, output_bbox_min);
        self.assert_array_almost_equal([100, 1e-3, 0],
                output_bbox_max - input_bbox_max);


    def test_chain(self):
        input_mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        path = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [1.0, 1.0, 0.0],
            [0.0, 1.0, 0.0],
            ]);

        output_mesh = minkowski_sum(input_mesh, path);
        self.assertTrue(output_mesh.is_closed());
        self.assertTrue(output_mesh.is_oriented());
        self.assertEqual(1, output_mesh.num_components);

        self_intersections = detect_self_intersection(output_mesh);
        self.assertEqual(0, len(self_intersections));
