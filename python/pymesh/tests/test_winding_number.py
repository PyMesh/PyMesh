from pymesh.TestCase import TestCase

from pymesh import compute_winding_number
from pymesh.meshutils import generate_box_mesh

import numpy as np
from numpy.linalg import norm

class WindingNumberTest(TestCase):
    def test_cube(self):
        mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        queries = np.array([
            [-1, 0, 0],
            [0.0, 0.0, 0.0],
            [0.5, 0.0, 0.0],
            [0.5, 0.5, 0.0],
            [0.5, 0.5, 0.5],
            ]);

        winding_numbers = compute_winding_number(mesh, queries);

        self.assertEqual(len(queries), len(winding_numbers));
        self.assertAlmostEqual(0.0, norm(winding_numbers -
            np.array([0, 0.125, 0.25, 0.5, 1])));
