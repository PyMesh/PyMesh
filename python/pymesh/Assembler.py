import PyMesh

import numpy as np
from .material import Material

class Assembler:
    def __init__(self, mesh, material=None):
        if material is None:
            # Using dummy material.
            material = Material.create_isotropic(mesh.dim, 1.0, 1.0, 0.0);
        self.__mesh = mesh;
        self.__material = material;
        self.__raw_assembler = PyMesh.FEAssembler.create(
                mesh.raw_mesh, material.raw_material);

    def assemble(self, matrix_name):
        return self.__raw_assembler.assemble(matrix_name);

    @property
    def material(self):
        return self.__material;

    @material.setter
    def material(self, material):
        self.__material = material;
        self.__raw_assembler.set_material(material.raw_material);

    @property
    def mesh(self):
        return self.__mesh;
