import PyMesh
import numpy as np

def chain_edges(edges):
    """ Given a #E by 2 edge matrix. Return a list of edge chain/loops.
    """
    if not isinstance(edges, np.ndarray):
        edges = np.array(edges, dtype=int)
    return PyMesh.chain_edges(edges)
