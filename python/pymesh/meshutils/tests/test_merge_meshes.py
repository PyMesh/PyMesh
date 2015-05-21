from pymesh.meshutils import merge_meshes
from pymesh.TestCase import TestCase

import numpy as np

class MergeMeshesTest(TestCase):
    def test_2_meshes(self):
        mesh_1 = self.load_mesh("cube.obj")
        mesh_2 = self.load_mesh("tet.obj");
        out_mesh = merge_meshes([mesh_1, mesh_2]);

        self.assertEqual(mesh_1.num_vertices + mesh_2.num_vertices,
                out_mesh.num_vertices);
        self.assertEqual(mesh_1.num_faces + mesh_2.num_faces,
                out_mesh.num_faces);
        self.assertEqual(mesh_1.num_voxels + mesh_2.num_voxels,
                out_mesh.num_voxels);

    def test_3_meshes(self):
        mesh_1 = self.load_mesh("cube.obj")
        mesh_2 = self.load_mesh("tet.obj");
        mesh_3 = self.load_mesh("suzanne.obj");
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
        mesh_1 = self.load_mesh("square_2D.obj");
        out_mesh = merge_meshes([mesh_1] * 3);

        self.assertEqual(mesh_1.num_vertices * 3, out_mesh.num_vertices);
        self.assertEqual(mesh_1.num_faces * 3, out_mesh.num_faces);
        self.assertEqual(mesh_1.num_voxels * 3, out_mesh.num_voxels);
