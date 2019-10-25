import PyMesh

import numpy as np
from .material import Material

class Assembler:
    """ Finite element matrix assembler

    Example:

    >>> mesh = pymesh.generate_icosphere(1.0, np.zeros(3), 3)
    >>> assembler = pymesh.Assembler(mesh)
    >>> L = assembler.assemble("laplacian")
    >>> type(L)
    <class 'scipy.sparse.csc.csc_matrix'>

    This example assembles the Laplacian-Beltrami matrix used by many graphics
    applications.  Other types of finite element matrices include:

        * ``stiffness``
        * ``mass``
        * ``lumped_mass``
        * ``laplacian``
        * ``displacement_strain``
        * ``elasticity_tensor``
        * ``engineer_strain_stress``
        * ``rigid_motion``
        * ``gradient``
        * ``graph_laplacian``
    """
    def __init__(self, mesh, material=None):
        if material is None:
            # Using dummy material.
            material = Material.create_isotropic(mesh.dim, 1.0, 1.0, 0.0)
        self.__mesh = mesh
        self.__material = material
        self.__raw_assembler = PyMesh.FEAssembler.create(
                mesh.raw_mesh, material.raw_material)

    def assemble(self, matrix_name):
        return self.__raw_assembler.assemble(matrix_name)

    @property
    def material(self):
        return self.__material

    @material.setter
    def material(self, material):
        self.__material = material
        self.__raw_assembler.set_material(material.raw_material)

    @property
    def mesh(self):
        return self.__mesh
