from pymesh import Material
from pymesh.TestCase import TestCase

import numpy as np

class MaterialTest(TestCase):
    def assert_symmetry(self, tensor, dim):
        """ Check all symmetries of elasticity tensor.
        """
        C = tensor;
        for i in range(dim):
            for j in range(dim):
                for k in range(dim):
                    for l in range(dim):
                        self.assertAlmostEqual(C[i,j,k,l], C[j,i,k,l]);
                        self.assertAlmostEqual(C[i,j,k,l], C[i,j,l,k]);
                        self.assertAlmostEqual(C[i,j,k,l], C[k,l,i,j]);

    def test_isotropic_material(self):
        young = 1000.0;
        poisson = 0.3;
        mat = Material.create_isotropic(3, 1.0, young, poisson);
        self.assertEqual(3, mat.dim);
        self.assertEqual(1.0, mat.density);
        self.assert_symmetry(mat.material_tensor, mat.dim);

        strain = np.array([
            [1.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0], ]);

        stress = mat.strain_to_stress(strain);
        self.assertAlmostEqual(young * (1-poisson)/(1+poisson)/(1-2*poisson),
                stress[0, 0]);
        self.assertAlmostEqual(young * poisson /(1+poisson)/(1-2*poisson),
                stress[1, 1]);
        self.assertAlmostEqual(young * poisson /(1+poisson)/(1-2*poisson),
                stress[2, 2]);
        self.assertAlmostEqual(0.0, stress[0, 1]);
        self.assertAlmostEqual(0.0, stress[1, 0]);
        self.assertAlmostEqual(0.0, stress[0, 2]);
        self.assertAlmostEqual(0.0, stress[2, 0]);
        self.assertAlmostEqual(0.0, stress[1, 2]);
        self.assertAlmostEqual(0.0, stress[2, 1]);

    def test_isotropic_material_2D(self):
        young = 1000.0;
        poisson = 0.3;
        mat = Material.create_isotropic(2, 1.0, young, poisson);
        self.assertEqual(2, mat.dim);
        self.assertEqual(1.0, mat.density);
        self.assert_symmetry(mat.material_tensor, mat.dim);

        strain = np.array([ [1.0, 0.0], [0.0, 0.0] ]);
        stress = mat.strain_to_stress(strain);
        self.assertAlmostEqual(strain[0,0],
                stress[0,0]/young - poisson*stress[1,1]/young );
        self.assertAlmostEqual(strain[1,1],
                stress[1,1]/young - poisson*stress[0,0]/young );
        self.assertAlmostEqual(strain[0, 1],
                stress[0,1] * (1+poisson) / young);

    def test_equivalence(self):
        young = 1000.0;
        poisson = 0.3;
        iso_mat = Material.create_isotropic(3, 1.0, young, poisson);

        ortho_mat = Material.create_orthotropic(1.0,
                np.ones(3)*young,
                np.ones(6)*poisson,
                np.ones(3)*(0.5*young/(1+poisson)));
        self.assert_symmetry(ortho_mat.material_tensor, 3);

        self.assertEqual(iso_mat.dim, ortho_mat.dim);
        self.assertEqual(iso_mat.density, ortho_mat.density);
        self.assert_array_almost_equal(iso_mat.material_tensor,
                ortho_mat.material_tensor);


