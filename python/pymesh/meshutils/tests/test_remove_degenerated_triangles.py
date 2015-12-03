from pymesh import remove_degenerated_triangles
from pymesh import remove_degenerated_triangles_raw
from pymesh import generate_box_mesh
from pymesh.TestCase import TestCase

import numpy as np

class RemoveDegeneratedTrianglesTest(TestCase):
    def test_no_degeneracy(self):
        mesh = generate_box_mesh(np.ones(3)*-1, np.ones(3));

        result, info = remove_degenerated_triangles(mesh);
        self.assertEqual(8, result.num_vertices);
        self.assertEqual(12, result.num_faces);
        self.assert_array_equal(np.arange(12),
                sorted(info["ori_face_indices"]));

    def test_simple(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            ]);
        faces = np.array([
            [0, 1, 2]
            ], dtype=int);

        result_vertices, result_faces, info = \
                remove_degenerated_triangles_raw(vertices, faces);

        self.assertEqual(0, len(result_vertices));
        self.assertEqual(0, len(result_faces));
        self.assertEqual(0, len(info["ori_face_indices"]));

    def multiple_degeneracies(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [2.0, 0.0, 0.0],
            [4.0, 0.0, 0.0],
            [5.0, 0.0, 0.0],
            [0.0, 2.0, 0.0],
            ]);

        faces = np.array([
            [0, 1, 5],
            [1, 2, 5],
            [2, 3, 5],
            [3, 4, 5],
            [5, 6, 0],
            ], dtype=int);

        result_vertices, result_faces, info = \
                remove_degenerated_triangles_raw(vertices, faces);

        self.assertEqual(6, len(result_vertices));
        self.assertEqual(4, len(result_faces));
        self.assertEqual(4, len(info["ori_face_indices"]));
        self.assertTrue(np.all(info["ori_face_indices"] == 4));

