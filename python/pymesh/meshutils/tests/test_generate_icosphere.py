from pymesh.meshutils import generate_icosphere
from pymesh.TestCase import TestCase

import numpy as np
from numpy.linalg import norm

class GenerateIcosphereTest(TestCase):
    def test_simple(self):
        mesh = generate_icosphere(1.0, center=np.zeros(3));
        center = np.average(mesh.bbox, axis=0);
        dist_to_center = norm(mesh.vertices - center, axis=1);
        self.assertAlmostEqual(1.0, np.amax(dist_to_center));
        self.assertAlmostEqual(1.0, np.amin(dist_to_center));
        self.assertTrue(mesh.is_oriented());

    def test_with_offset(self):
        mesh = generate_icosphere(1.0, center=np.array([1.0, 1.5, -1.0]));
        center = np.average(mesh.bbox, axis=0);
        self.assert_array_equal([1.0, 1.5, -1.0], center);
        dist_to_center = norm(mesh.vertices - center, axis=1);
        self.assertAlmostEqual(1.0, np.amax(dist_to_center));
        self.assertAlmostEqual(1.0, np.amin(dist_to_center));
        self.assertTrue(mesh.is_oriented());

    def test_with_offset_and_refinement(self):
        mesh = generate_icosphere(1.0, center=np.array([1.0, 1.5, -1.0]),
                refinement_order=2);
        self.assertLess(12, mesh.num_vertices);
        center = np.average(mesh.bbox, axis=0);
        self.assert_array_equal([1.0, 1.5, -1.0], center);
        dist_to_center = norm(mesh.vertices - center, axis=1);
        self.assertAlmostEqual(1.0, np.amax(dist_to_center));
        self.assertAlmostEqual(1.0, np.amin(dist_to_center));
        self.assertTrue(mesh.is_oriented());
