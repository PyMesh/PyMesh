from pymesh import separate_mesh
from pymesh import merge_meshes
from pymesh import form_mesh
from pymesh import generate_box_mesh
from pymesh.TestCase import TestCase

import numpy as np

class SeparateMeshTest(TestCase):
    def test_simple(self):
        mesh_1 = generate_box_mesh(np.zeros(3), np.ones(3));
        mesh_2 = generate_box_mesh(np.array([0.5, 0.5, 0.5]), np.ones(3));
        out_mesh = merge_meshes([mesh_1, mesh_2]);

        components = separate_mesh(out_mesh);
        self.assertEqual(2, len(components));

        for comp in components:
            self.assertEqual(8, comp.num_vertices);
            self.assertEqual(12, comp.num_faces);

    def test_face_connectivity(self):
        vertices = np.array([
            [0, 0, 0],
            [1, 0, 0],
            [0, 1, 0],
            [0, 0, 1],
            [1, 1, 1],
            ], dtype=float);

        faces = np.array([
            [0, 1, 2],
            [2, 3, 4],
            ]);

        mesh = form_mesh(vertices, faces);
        components = separate_mesh(mesh, "vertex");
        self.assertEqual(1, len(components));

        components = separate_mesh(mesh, "face");
        self.assertEqual(2, len(components));
