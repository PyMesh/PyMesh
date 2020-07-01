import pymesh
from pymesh.TestCase import TestCase

import numpy as np
import unittest

class TetgenTest(TestCase):
    def test_point_cloud(self):
        tetgen = pymesh.tetgen()
        tetgen.points = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [1.0, 1.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            [1.0, 0.0, 1.0],
            [1.0, 1.0, 1.0],
            [0.0, 1.0, 1.0] ])
        tetgen.verbosity = 0
        tetgen.keep_convex_hull = True
        tetgen.split_boundary = False
        tetgen.run()

        mesh = tetgen.mesh
        mesh.add_attribute("voxel_volume")
        volumes = mesh.get_attribute("voxel_volume")

        self.assertEqual(8, len(tetgen.vertices))
        self.assertAlmostEqual(1.0, np.sum(volumes))

    def test_cube_refine(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [1.0, 1.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            [1.0, 0.0, 1.0],
            [1.0, 1.0, 1.0],
            [0.0, 1.0, 1.0] ])
        # A cube can be splitted into 5 tets or 6 tets without adding steiner
        # points.  We start with 5 tets and see if tetgen can refine it to the 6
        # tet configuration.
        tets = np.array([
            [0, 1, 2, 5],
            [0, 2, 3, 7],
            [4, 7, 5, 0],
            [5, 7, 6, 2],
            [0, 5, 2, 7]
            ], dtype=int)
        mesh = pymesh.form_mesh(vertices, np.array([]), tets)
        mesh.add_attribute("voxel_volume")
        volumes = mesh.get_attribute("voxel_volume")
        self.assertAlmostEqual(1.0, np.sum(volumes))

        tetgen = pymesh.tetgen()
        tetgen.points = mesh.vertices
        tetgen.triangles = mesh.faces
        tetgen.tetrahedra = mesh.voxels
        tetgen.max_tet_volume = 0.17
        tetgen.max_num_steiner_points = 0
        tetgen.split_boundary = False
        tetgen.verbosity = 0
        tetgen.run()

        mesh = tetgen.mesh
        mesh.add_attribute("voxel_volume")
        volumes = mesh.get_attribute("voxel_volume")
        self.assertAlmostEqual(1.0, np.sum(volumes))

        # No tetgen cannot recover the 6 tet configuration...
        # So tet_volume constaint is actually not satisfied by the result.
        self.assertEqual(8, len(tetgen.vertices))
        self.assertEqual(5, len(tetgen.voxels))

    def test_volume_constraint(self):
        tetgen = pymesh.tetgen()
        tetgen.points = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [1.0, 1.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            [1.0, 0.0, 1.0],
            [1.0, 1.0, 1.0],
            [0.0, 1.0, 1.0] ])
        tetgen.triangles = np.array([
            [0, 2, 1],
            [0, 3, 2],
            [5, 6, 7],
            [5, 7, 4],
            [1, 2, 5],
            [2, 6, 5],
            [0, 4, 3],
            [3, 4, 7],
            [0, 1, 5],
            [0, 5, 4],
            [2, 3, 6],
            [3, 7, 6], ])
        tetgen.verbosity = 0
        tetgen.keep_convex_hull = True
        tetgen.split_boundary = True
        tetgen.max_tet_volume = 0.1
        tetgen.run()

        mesh = tetgen.mesh
        mesh.add_attribute("voxel_volume")
        volumes = mesh.get_attribute("voxel_volume")

        self.assertLess(8, len(tetgen.vertices))
        self.assertLess(6, len(tetgen.voxels))
        self.assertAlmostEqual(1.0, np.sum(volumes))

    @unittest.skip("debug")
    def test_weighted(self):
        tetgen = pymesh.tetgen()
        tetgen.points = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            [0.1, 0.1, 0.1]
            ])
        tetgen.point_weights = np.array([1.0, 1.0, 1.0, 1.0, 0.0])
        tetgen.verbosity = 0
        tetgen.weighted_delaunay = True
        tetgen.run()

        self.assertEqual(5, len(tetgen.vertices))
        self.assertEqual(1, len(tetgen.voxels))

        # Rerun but not weighted.
        tetgen.verbosity = 0
        tetgen.weighted_delaunay = False
        tetgen.run()

        self.assertEqual(5, len(tetgen.vertices))
        self.assertEqual(4, len(tetgen.voxels))

        # Rerun with 0 weights.
        tetgen.point_weights = np.array([0.0, 0.0, 0.0, 0.0, 0.0])
        tetgen.verbosity = 0
        tetgen.weighted_delaunay = True
        tetgen.run()

        self.assertEqual(5, len(tetgen.vertices))
        self.assertEqual(4, len(tetgen.voxels))

