from pymesh import separate_mesh
from pymesh import merge_meshes
from pymesh import form_mesh
from pymesh.TestCase import TestCase

import numpy as np

class SeparateMeshTest(TestCase):
    def test_simple(self):
        mesh_1 = self.load_mesh("cube.obj")
        mesh_2 = self.load_mesh("tet.obj");
        out_mesh = merge_meshes([mesh_1, mesh_2]);

        components = separate_mesh(out_mesh);
        self.assertEqual(2, len(components));

        for comp in components:
            if comp.num_vertices == 4:
                self.assertEqual(mesh_2.num_faces, comp.num_faces);
                self.assert_array_equal(mesh_2.bbox, comp.bbox);
            else:
                self.assertEqual(mesh_1.num_faces, comp.num_faces);
                self.assert_array_equal(mesh_1.bbox, comp.bbox);

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
