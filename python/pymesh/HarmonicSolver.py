import PyMesh
from .meshio import form_mesh

class HarmonicSolver(PyMesh.HarmonicSolver):
    """ Harmonic PDE solver.

    Input attributes:
        nodes (:class:``numpy.ndarray``): List of 2D or 3D nodes.
        elements (:class:``numpy.ndarray``): List of triangles (2D) or tets (3D).
        order (:class:``numpy.ndarray``): Order of harmonic function.  1 is
            regular harmonic equation, 2 is biharmonic equation, etc.  Default
            is 1.
        boundary_indices (:class:``numpy.ndarray``): Indices into ``nodes``,
            specifying boundary nodes.
        boundary_values (:class:``numpy.ndarray``): Function values associated
            with the boundary node.

    Methods:
        pre_process(): Run preprocessing step.  (All input attribute must be
            specified prior of calling this method.)
        solve(): Run solver.

    Output attributes:
        solution (:class:``numpy.ndarray``): Solution for the specified harmonic
            PDE.  (i.e. Solved functions evaluated on the input nodes.)

    Example:
        >>> solver = pymesh.HarmonicSolver.create(mesh)
        >>> solver.bd_indices = np.unique(mesh.faces.ravel())
        >>> solver.bd_values = ... # Set the corresponding value.
        >>> solver.pre_process()
        >>> solver.solve()
        >>> sol = solver.solution
    """

    @classmethod
    def create(cls, mesh):
        """ Create a harmonic solver using the input mesh as domain.
        """
        return PyMesh.HarmonicSolver.create(mesh.raw_mesh)

    @property
    def mesh(self):
        nodes = self.nodes
        elements = self.elements
        if nodes.shape[1] == 2:
            assert(elements.shape[1] == 3)
            return form_mesh(nodes, elements)
        else:
            assert(nodes.shape[1] == 3)
            assert(elements.shape[1] == 4)
            return form_mesh(nodes, np.array([]), elements)

    @mesh.setter
    def mesh(self, mesh):
        dim = mesh.dim
        if dim == 2:
            self.nodes = mesh.vertices
            self.elements = mesh.faces
        else:
            assert(dim == 3)
            assert(mesh.num_voxels > 0)
            self.nodes = mesh.vertices
            self.elements = mesh.voxels

