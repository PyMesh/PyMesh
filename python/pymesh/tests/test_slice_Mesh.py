from pymesh.TestCase import TestCase
from pymesh import slice_mesh, merge_meshes
from pymesh.meshutils import generate_box_mesh

import numpy as np

class SliceMeshTest(TestCase):
    def assert_bbox_is_embedded(self, bbox_1, bbox_2):
        self.assertGreaterEqual(bbox_1[0][0], bbox_2[0][0]);
        self.assertGreaterEqual(bbox_1[0][1], bbox_2[0][1]);
        self.assertGreaterEqual(bbox_1[0][2], bbox_2[0][2]);

        self.assertLessEqual(bbox_1[1][0], bbox_2[1][0]);
        self.assertLessEqual(bbox_1[1][1], bbox_2[1][1]);
        self.assertLessEqual(bbox_1[1][2], bbox_2[1][2]);

    def test_box(self):
        mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        N = 2;

        slices = slice_mesh(mesh, [1, 0, 0], N);
        self.assertEqual(N, len(slices));

        slices = merge_meshes(slices);
        self.assert_bbox_is_embedded(slices.bbox, mesh.bbox);

    def test_diag_box(self):
        mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        N = 5;

        slices = slice_mesh(mesh, [1, 1, 1], N);
        self.assertEqual(N, len(slices));

        slices = merge_meshes(slices);
        self.assert_bbox_is_embedded(slices.bbox, mesh.bbox);
