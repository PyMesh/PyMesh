from pymesh.meshutils import quad_to_tri
from pymesh.TestCase import TestCase

import numpy as np

class QuadToTriTest(TestCase):
    def test_single_quad(self):
        vertices = np.array([
            [0, 0, 0],
            [1, 0, 0],
            [1, 1, 0],
            [0, 1, 0],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2, 3]
            ]);
        quad_mesh = self.form_mesh(vertices, faces);
        tri_mesh = quad_to_tri(quad_mesh);
        self.assertEqual(3, tri_mesh.vertex_per_face);
        self.assertEqual(2, tri_mesh.num_faces);
        self.assert_array_equal(quad_mesh.bbox, tri_mesh.bbox);

        self.assertTrue(tri_mesh.has_attribute("cell_index"));
        cell_index = tri_mesh.get_attribute("cell_index").ravel();
        self.assertTrue(np.all(cell_index == 0));

    def test_single_quad_symmetric(self):
        vertices = np.array([
            [0, 0, 0],
            [1, 0, 0],
            [1, 1, 0],
            [0, 1, 0],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2, 3]
            ]);
        quad_mesh = self.form_mesh(vertices, faces);
        tri_mesh = quad_to_tri(quad_mesh, keep_symmetry = True);
        self.assertEqual(3, tri_mesh.vertex_per_face);
        self.assertEqual(4, tri_mesh.num_faces);
        self.assert_array_equal(quad_mesh.bbox, tri_mesh.bbox);

        self.assertTrue(tri_mesh.has_attribute("cell_index"));
        cell_index = tri_mesh.get_attribute("cell_index").ravel();
        self.assertTrue(np.all(cell_index == 0));

