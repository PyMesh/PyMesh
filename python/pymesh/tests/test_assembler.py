from pymesh import Assembler, form_mesh, Material
from pymesh.TestCase import TestCase

import numpy as np
import scipy.sparse.linalg

class AssemblerTest(TestCase):
    def single_triangle_2D_setup(self):
        vertices = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [0.0, 1.0],
            ])
        faces = np.array([
            [0, 1, 2]
            ])
        mesh = form_mesh(vertices, faces)
        material = Material.create_isotropic(2, 1.0, 1.0, 0.0)
        assembler = Assembler(mesh, material)
        return assembler

    def assert_symmetry(self, A):
        self.assertAlmostEqual(0.0, scipy.sparse.linalg.norm(A - A.T))

    def assert_PD(self, A):
        eig_values, eig_vectors = scipy.sparse.linalg.eigsh(A, 1, which="SM")
        self.assertLess(0.0, eig_values[0])

    def assert_PSD(self, A):
        eig_values, eig_vectors = scipy.sparse.linalg.eigsh(A, 1, which="SA")
        eps = np.spacing((A.max() - A.min()) ** 2 * A.shape[0])
        self.assertLessEqual(0.0, eps + eig_values[0])

    def test_mass_matrix(self):
        assembler = self.single_triangle_2D_setup()
        M = assembler.assemble("mass")
        self.assert_symmetry(M)
        self.assert_PD(M)

        # integrate a constant field over the mesh should yield the area.
        v = np.ones(3)
        mass = np.dot(v, (M * v))
        self.assertAlmostEqual(0.5, mass)

    def test_lumped_mass_matrix(self):
        assembler = self.single_triangle_2D_setup()
        M = assembler.assemble("lumped_mass")
        self.assert_symmetry(M)
        self.assert_PD(M)

        # integrate a constant field over the mesh should yield the area.
        v = np.ones(3)
        mass = np.dot(v, (M * v))
        self.assertAlmostEqual(0.5, mass)

    def test_stiffness_matrix(self):
        assembler = self.single_triangle_2D_setup()
        K = assembler.assemble("stiffness")
        self.assert_symmetry(K)
        self.assert_PSD(K)

    def test_elasticity_tensor(self):
        assembler = self.single_triangle_2D_setup()
        E = assembler.assemble("elasticity_tensor")
        self.assert_symmetry(E)
        self.assert_PD(E)

        # The default material should yield identity elasticity tensor.
        I = scipy.sparse.eye(assembler.mesh.dim * (assembler.mesh.dim + 1) / 2)
        self.assertAlmostEqual(0.0, scipy.sparse.linalg.norm(E-I))


