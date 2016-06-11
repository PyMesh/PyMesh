import PyAssembler

import numpy as np

class Material:
    @classmethod
    def create_isotropic(cls, dim, density, young, poisson):
        """ Create an isotropic material.

        Args:
            dim: Dimension of the ambient space (must be 2 or 3).
            density: Material density.
            young: Young's modulus.
            poisson: Poisson's ratio
                     (must be in (-1, 0.5) in 3D and (-1, 1) in 2D).
        """
        return Material(PyAssembler.Material.create_isotropic(
            dim, density, young, poisson));

    @classmethod
    def create_orthotropic(cls, density, young, poisson, shear):
        """ Create an orthotropic material.

        Args:
            desnity: Material density.
            young: Array of Young's modulus, [young_x, young_y, young_z]
            poisson: Array of Poisson's ratio, [poisson_yz, poisson_zy,
                                                poisson_zx, poisson_xz,
                                                poisson_xy, poisson_yx]
            shear: Array of Shear modulus, [shear_yz, shear_zx, shear_xy]
        """
        return Material(PyAssembler.Material.create_orthotropic(
            density, young, poisson, shear));

    def __init__(self, raw_material=None):
        self.raw_material = raw_material;

    def strain_to_stress(self, strain, coord=None):
        if coord is None:
            coord = np.zeros(self.dim);
        return self.raw_material.strain_to_stress(strain, coord);

    @property
    def density(self):
        """
        Density at the origin.
        """
        return self.raw_material.get_density();

    @property
    def material_tensor(self):
        """
        Return 4th order material tensor of size d x d x d x d evaluated at the
        origin.
        """
        coord = np.zeros(self.dim);
        tensor = np.empty([self.dim, self.dim, self.dim, self.dim]);
        indices = np.arange(self.dim);
        I,J,K,L = np.meshgrid(indices, indices, indices, indices);
        for i,j,k,l in zip(I.ravel(), J.ravel(), K.ravel(), L.ravel()):
            tensor[i,j,k,l] = self.raw_material.get_material_tensor(
                    int(i),int(j),int(k),int(l),coord);
        return tensor;

    @property
    def dim(self):
        """ Dimension of the ambient space.
        """
        return self.raw_material.get_dim();
