import pymesh
from pymesh.TestCase import TestCase

import numpy as np
import numpy.linalg

class HarmonicSolverTest(TestCase):
    def test_linear_function(self):
        mesh = pymesh.generate_icosphere(1.0, np.zeros(3), 2)
        tetgen = pymesh.tetgen()
        tetgen.points = mesh.vertices
        tetgen.triangles = mesh.faces
        tetgen.max_tet_volume = 0.1
        tetgen.verbosity = 0
        tetgen.run()

        mesh = tetgen.mesh
        self.assertLess(0, mesh.num_voxels)

        # Test solver to reproduce linear coordinate functions.
        solver = pymesh.HarmonicSolver.create(mesh)
        bd_indices = np.unique(mesh.faces.ravel())
        bd_values = mesh.vertices[bd_indices, 0]
        solver.boundary_indices = bd_indices
        solver.boundary_values = bd_values
        solver.pre_process()
        solver.solve()

        sol = solver.solution.ravel()

        self.assertEqual(mesh.num_vertices, len(sol))
        self.assert_array_almost_equal(mesh.vertices[:, 0], sol, 12)

    def test_radial_function(self):
        mesh = pymesh.generate_icosphere(1.0, [1.0, 1.0, 1.0], 2)
        tetgen = pymesh.tetgen()
        tetgen.points = mesh.vertices
        tetgen.triangles = mesh.faces
        tetgen.max_tet_volume = 0.001
        tetgen.verbosity = 0
        tetgen.run()

        mesh = tetgen.mesh
        self.assertLess(0, mesh.num_voxels)

        # Test solver to reproduce linear coordinate functions.
        solver = pymesh.HarmonicSolver.create(mesh)
        bd_indices = np.unique(mesh.faces.ravel())
        target_solution = 1.0 / numpy.linalg.norm(mesh.vertices, axis=1)
        bd_values = target_solution[bd_indices]
        solver.boundary_indices = bd_indices
        solver.boundary_values = bd_values
        solver.order = 1
        solver.pre_process()
        solver.solve()

        sol = solver.solution.ravel()

        #mesh.add_attribute("target_solution")
        #mesh.set_attribute("target_solution", target_solution)
        #mesh.add_attribute("solution")
        #mesh.set_attribute("solution", sol)
        #pymesh.save_mesh("tmp.msh", mesh, *mesh.attribute_names)

        self.assertEqual(mesh.num_vertices, len(sol))
        self.assert_array_almost_equal(target_solution, sol, 2)
        # TODO: accuracy is not great...

    def test_2D(self):
        tri = pymesh.triangle()
        tri.points = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [0.0, 1.0],
            [1.0, 1.0],
            ])
        tri.keep_convex_hull = True
        tri.max_area = 0.0001
        tri.verbosity = 0
        tri.run()

        mesh = tri.mesh
        self.assertLess(0, mesh.num_faces)

        solver = pymesh.HarmonicSolver.create(mesh)
        bd_indices = mesh.boundary_vertices.ravel()
        # f(x,y) = ln(x^2 + y^2) is known to be harmonic.
        target_solution = np.log(np.square(numpy.linalg.norm(mesh.vertices +
            1.0, axis=1)))
        bd_values = target_solution[bd_indices]

        solver.boundary_indices = bd_indices
        solver.boundary_values = bd_values
        solver.order = 1
        solver.pre_process()
        solver.solve()

        sol = solver.solution.ravel()

        #mesh.add_attribute("target_solution")
        #mesh.set_attribute("target_solution", target_solution)
        #mesh.add_attribute("solution")
        #mesh.set_attribute("solution", sol)
        #pymesh.save_mesh("tmp.msh", mesh, *mesh.attribute_names)

        self.assertEqual(mesh.num_vertices, len(sol))
        self.assert_array_almost_equal(target_solution, sol, 3)
