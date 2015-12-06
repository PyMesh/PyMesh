from pymesh.TestCase import TestCase
from pymesh import orient_3D, orient_2D, in_circle, in_sphere

import numpy as np

class PredicatesTest(TestCase):
    def test_positive_oriented_3D(self):
        p1 = np.array([0, 0, 0]);
        p2 = np.array([1, 0, 0]);
        p3 = np.array([0, 1, 0]);
        p4 = np.array([0, 0, 1]);

        self.assertGreater(0.0, orient_3D(p1, p2, p3, p4));

    def test_negative_oriented_3D(self):
        p1 = np.array([0, 0, 0]);
        p2 = np.array([1, 0, 0]);
        p3 = np.array([0, 1, 0]);
        p4 = np.array([0, 0,-1]);

        self.assertLess(0.0, orient_3D(p1, p2, p3, p4));

    def test_coplanar_3D(self):
        p1 = np.array([0, 0, 0]);
        p2 = np.array([1, 0, 0]);
        p3 = np.array([0, 1, 0]);
        p4 = np.array([0, 1, 0]);

        self.assertEqual(0.0, orient_3D(p1, p2, p3, p4));

    def test_collinear_3D(self):
        p1 = np.array([1, 0, 0]);
        p2 = np.array([1, 0, 0]);
        p3 = np.array([0, 1, 0]);
        p4 = np.array([0, 1, 0]);

        self.assertEqual(0.0, orient_3D(p1, p2, p3, p4));

    def test_degenerated_to_a_point_3D(self):
        p1 = np.array([1, 0, 0]);
        p2 = np.array([1, 0, 0]);
        p3 = np.array([1, 0, 0]);
        p4 = np.array([1, 0, 0]);

        self.assertEqual(0.0, orient_3D(p1, p2, p3, p4));

    def test_list_input_3D(self):
        p1 = [0.0, 0.0, 0.0];
        p2 = [1.0, 0.0, 0.0];
        p3 = [0.0, 1.0, 0.0];
        p4 = [0.0, 0.0, 1.0];

        self.assertGreater(0.0, orient_3D(p1, p2, p3, p4));

    def test_orientation_2D(self):
        p1 = np.array([0, 0]);
        p2 = np.array([1, 0]);
        p3 = np.array([0, 2]);

        self.assertLess(0.0, orient_2D(p1, p2, p3));
        self.assertGreater(0.0, orient_2D(p1, p3, p2));
        self.assertEqual(0.0, orient_2D(p1, p1, p2));

    def test_in_circle(self):
        p1 = np.array([0, 0]);
        p2 = np.array([1, 0]);
        p3 = np.array([0, 1]);
        p_in = np.array([0.5, 0.5]);
        p_out = np.array([2, 2]);
        p_on = np.array([1, 1]);

        self.assertLess(0.0, in_circle(p1, p2, p3, p_in));
        self.assertGreater(0.0, in_circle(p1, p2, p3, p_out));
        self.assertEqual(0.0, in_circle(p1, p2, p3, p_on));

    def test_in_sphere(self):
        p1 = [-1.0, 0.0, 0.0];
        p2 = [1.0, 0.0, 0.0];
        p3 = [0.0, 1.0, 0.0];
        p4 = [0.0, 0.0,-1.0];
        p_in = [0, 0, 0];
        p_out = [10, 0, 0];
        p_on = [0, -1, 0];

        self.assertLess(0.0, in_sphere(p1, p2, p3, p4, p_in));
        self.assertGreater(0.0, in_sphere(p1, p2, p3, p4, p_out));
        self.assertEqual(0.0, in_sphere(p1, p2, p3, p4, p_on));
