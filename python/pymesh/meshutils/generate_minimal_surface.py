import math
import numpy as np
import numpy.linalg

from ..SparseSolver import SparseSolver
from ..triangle import triangle
from ..Assembler import Assembler
from ..meshio import form_mesh

def solve_harmonic(L, bd_indices, bd_values):
    N = L.shape[0]
    m = bd_values.shape[1]
    x = np.zeros((N, m))
    x[bd_indices,:] = bd_values
    rhs = -L * x

    mask = np.ones(N, dtype=bool)
    mask[bd_indices] = False
    L = L[mask,:][:, mask]
    rhs = rhs[mask,:]

    solver = SparseSolver.create("LDLT")
    solver.compute(L)
    sol = solver.solve(rhs)

    full_solution = np.zeros((N, m))
    full_solution[bd_indices, :] = bd_values
    full_solution[mask, :] = sol

    return full_solution

def generate_minimal_surface(loop, resolution=10):
    """ Generate a minimal surface (e.g. soap membrane) to fill the region
    bounded by ``loop``.

    Args:
        loop: (``numpy.ndarray``): A N by dim array of vertices forming a simple
        closed loop.
        resolution (``int``): (optional) How many times to divide the surface.
        Higher resolution means finer triangulation.  Default is 10.

    Returns:
        A :py:class:`Mesh` representing the minimal surface that has the input
        ``loop`` as boundary.
    """
    N = len(loop)
    arc_lengths = np.zeros(N)
    for i in range(1,N):
        arc_lengths[i] = arc_lengths[i-1] + \
                numpy.linalg.norm(loop[i-1] - loop[i])
    L = arc_lengths[-1] + numpy.linalg.norm(loop[0] - loop[-1])

    thetas = arc_lengths / L * 2 * math.pi
    x = np.cos(thetas)
    y = np.sin(thetas)
    pts = np.array([x,y]).T
    idx = np.arange(N, dtype=int)
    edges = np.array([idx, np.roll(idx, -1)]).T

    tri = triangle()
    tri.points = pts
    tri.segments = edges
    tri.max_area = (1.0 / resolution)**2
    tri.split_boundary = False
    tri.run()

    domain = tri.mesh

    assembler = Assembler(domain)
    L = assembler.assemble("laplacian")

    pts = solve_harmonic(L, np.arange(N, dtype=int), loop)
    surface = form_mesh(pts, domain.faces)
    return surface

