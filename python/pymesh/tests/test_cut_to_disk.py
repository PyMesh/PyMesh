from pymesh.TestCase import TestCase
from pymesh import cut_to_disk
from pymesh import generate_icosphere, generate_tube, form_mesh

import numpy as np
import unittest

class DiskCutterTest(TestCase):
    def test_cut_sphere(self):
        mesh = generate_icosphere(1.0, np.zeros(3))
        self.assertEqual(2, mesh.euler_characteristic)
        mesh = cut_to_disk(mesh)
        num_bd_loops = mesh.num_boundary_loops
        self.assertEqual(0, num_bd_loops)
        self.assertEqual(2-num_bd_loops, mesh.euler_characteristic)

    def test_cut_tube(self):
        mesh = generate_tube(np.zeros(3), np.ones(3), 1.0, 0.5, 1.0, 0.5)
        self.assertEqual(0, mesh.euler_characteristic)
        mesh = cut_to_disk(mesh)
        num_bd_loops = mesh.num_boundary_loops
        self.assertEqual(1, num_bd_loops)
        self.assertEqual(2-num_bd_loops, mesh.euler_characteristic)

    @unittest.expectedFailure
    def test_cut_nonmanifold(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.5, 0.0, 1.0],
            [0.5, 1.0, 0.0],
            [0.5, 1.0, 1.0],
            ])
        faces = np.array([
            [0, 1, 2],
            [0, 1, 3],
            [0, 1, 4],
            ])
        mesh = form_mesh(vertices, faces)
        self.assertFalse(mesh.is_manifold())

        # Note that nonmanifold input mesh is not supported.
        mesh = cut_to_disk(mesh)
        self.assertTrue(mesh.is_manifold())
