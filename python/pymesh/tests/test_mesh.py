from pymesh.TestCase import TestCase
import pymesh
import numpy as np

class MeshTest(TestCase):
    def test_tet_connectivity(self):
        vertices = np.array([
            [0, 0, 0],
            [1, 0, 0],
            [0, 1, 0],
            [1, 1, 0],
            [0, 0, 1] ]);
        voxels = np.array([
            [0, 2, 1, 4],
            [1, 2, 3, 4]
            ]);
        mesh = pymesh.form_mesh(vertices, np.array([]), voxels);
        mesh.enable_connectivity();

        self.assertEqual(5, mesh.num_vertices);
        self.assertEqual(6, mesh.num_faces);
        self.assertEqual(2, mesh.num_voxels);

        self.assert_array_equal([1], mesh.get_voxel_adjacent_voxels(0));
        self.assert_array_equal([0], mesh.get_voxel_adjacent_voxels(1));

    def test_hex_connectivity(self):
        mesh = pymesh.generate_box_mesh([0.0, 0.0, 0.0], [1.0, 1.0, 1.0],
                num_samples=2,
                using_simplex=False);
        mesh.enable_connectivity();

        self.assertEqual(27, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertEqual(8, mesh.num_voxels);

        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(0)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(1)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(2)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(3)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(4)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(5)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(6)));
        self.assertEqual(3, len(mesh.get_voxel_adjacent_voxels(7)));
