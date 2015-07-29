from pymesh.meshutils import hex_to_tet
from pymesh.meshutils import generate_box_mesh
from pymesh.TestCase import TestCase

import numpy as np

class HexToTetTest(TestCase):
    def test_single_hex(self):
        hex_mesh = generate_box_mesh(
                np.zeros(3),
                np.ones(3),
                1, using_simplex=False);
        tet_mesh = hex_to_tet(hex_mesh, True, 0);
        self.assert_array_equal(hex_mesh.bbox, tet_mesh.bbox);
        self.assertEqual(24, tet_mesh.num_voxels);

    def test_multiple_hex(self):
        hex_mesh = generate_box_mesh(
                np.zeros(3),
                np.ones(3),
                2, using_simplex=False);
        tet_mesh = hex_to_tet(hex_mesh, True, 0);
        self.assert_array_equal(hex_mesh.bbox, tet_mesh.bbox);
        self.assertEqual(8 * 24, tet_mesh.num_voxels);
