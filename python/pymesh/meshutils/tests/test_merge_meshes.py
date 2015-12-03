from pymesh import generate_box_mesh
from pymesh import form_mesh
from pymesh import merge_meshes
from pymesh.TestCase import TestCase

import numpy as np

class MergeMeshesTest(TestCase):
    def test_2_meshes(self):
        mesh_1 = generate_box_mesh(np.zeros(3), np.ones(3));
        mesh_2 = generate_box_mesh(np.ones(3), np.ones(3)*2, subdiv_order=2);
        out_mesh = merge_meshes([mesh_1, mesh_2]);

        self.assertEqual(mesh_1.num_vertices + mesh_2.num_vertices,
                out_mesh.num_vertices);
        self.assertEqual(mesh_1.num_faces + mesh_2.num_faces,
                out_mesh.num_faces);
        self.assertEqual(mesh_1.num_voxels + mesh_2.num_voxels,
                out_mesh.num_voxels);

    def test_3_meshes(self):
        mesh_1 = generate_box_mesh(np.zeros(3), np.ones(3));
        mesh_2 = generate_box_mesh(np.ones(3), np.ones(3)*2, subdiv_order=2);
        mesh_3 = generate_box_mesh([0.5, 0.5, 0.5], [0.6, 0.6, 0.6]);
        out_mesh = merge_meshes([mesh_1, mesh_2, mesh_3]);

        self.assertEqual(
                mesh_1.num_vertices + mesh_2.num_vertices + mesh_3.num_vertices,
                out_mesh.num_vertices);
        self.assertEqual(
                mesh_1.num_faces + mesh_2.num_faces + mesh_3.num_faces,
                out_mesh.num_faces);
        self.assertEqual(
                mesh_1.num_voxels + mesh_2.num_voxels + mesh_3.num_voxels,
                out_mesh.num_voxels);

    def test_2D_meshes(self):
        vertices = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [1.0, 1.0],
            [0.0, 1.0] ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            [0, 2, 3] ], dtype=int);
        mesh_1 = form_mesh(vertices, faces);
        out_mesh = merge_meshes([mesh_1] * 3);

        self.assertEqual(mesh_1.num_vertices * 3, out_mesh.num_vertices);
        self.assertEqual(mesh_1.num_faces * 3, out_mesh.num_faces);
        self.assertEqual(mesh_1.num_voxels * 3, out_mesh.num_voxels);
