from pymesh.TestCase import TestCase
from pymesh import distance_to_mesh
from pymesh.meshutils import generate_box_mesh

import numpy as np

class DistanceToMeshTest(TestCase):
    def test_boundary_pts(self):
        mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        pts = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0] ]);

        sq_dist, face_idx, closest_pts = distance_to_mesh(mesh, pts);
        self.assert_array_equal(sq_dist, np.zeros(2));
