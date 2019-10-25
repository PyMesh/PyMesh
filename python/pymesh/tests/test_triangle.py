import pymesh
from pymesh.TestCase import TestCase

try:
    from pymesh import triangle
    import numpy as np
except:
    pass
else:
    class TriangleTest(TestCase):
        def test_point_cloud(self):
            tri = triangle()
            tri.points = np.array([
                [0.0, 0.0],
                [1.0, 0.0],
                [1.0, 1.0],
                [0.0, 1.0] ])
            tri.verbosity = 0
            tri.split_boundary = False
            tri.max_num_steiner_points = 0
            tri.run()

            self.assertEqual(4, len(tri.vertices))
            self.assertEqual(2, len(tri.faces))

        def test_pslg(self):
            tri = triangle()
            tri.points = np.array([
                [0.0, 0.0],
                [1.0, 0.0],
                [1.0, 1.0],
                [0.0, 1.0] ])
            tri.segments = np.array([
                [0, 2],
                [1, 3] ])
            tri.verbosity = 0
            tri.split_boundary = False
            tri.max_num_steiner_points = 0
            tri.keep_convex_hull = True
            tri.run()

            self.assertEqual(5, len(tri.vertices))
            self.assertEqual(4, len(tri.faces))

        def test_convex_hull(self):
            tri = triangle()
            tri.points = np.array([
                [0.0, 0.0],
                [1.0, 0.0],
                [0.1, 0.1],
                [0.0, 1.0] ])
            tri.keep_convex_hull = True
            tri.verbosity = 0
            tri.max_num_steiner_points = 0
            tri.run()

            self.assertEqual(4, len(tri.vertices))
            self.assertEqual(3, len(tri.faces))

        def test_holes(self):
            tri = triangle()
            tri.points = np.array([
                [0.0, 0.0],
                [1.0, 0.0],
                [1.0, 1.0],
                [0.0, 1.0],
                [0.2, 0.2],
                [0.8, 0.2],
                [0.8, 0.8],
                [0.2, 0.8] ])
            tri.segments = np.array([
                [0, 1],
                [1, 2],
                [2, 3],
                [3, 0],
                [5, 4],
                [6, 5],
                [7, 6],
                [4, 7] ])
            tri.holes = np.array([
                [0.5, 0.5]
                ])
            tri.verbosity = 0
            tri.run()
            mesh = tri.mesh
            self.assertEqual(2, mesh.num_boundary_loops)

        def test_auto_hole_detection(self):
            tri = triangle()
            tri.points = np.array([
                [0.0, 0.0],
                [1.0, 0.0],
                [1.0, 1.0],
                [0.0, 1.0],
                [0.2, 0.2],
                [0.8, 0.2],
                [0.8, 0.8],
                [0.2, 0.8] ])
            tri.segments = np.array([
                [0, 1],
                [1, 2],
                [2, 3],
                [3, 0],
                [5, 4],
                [6, 5],
                [7, 6],
                [4, 7] ])
            tri.auto_hole_detection = True
            tri.verbosity = 0
            tri.run()
            mesh = tri.mesh
            self.assertEqual(2, mesh.num_boundary_loops)
