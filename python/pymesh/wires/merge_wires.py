from . import WireNetwork
from . import form_wires
import numpy as np

def merge_wires(wire_networks):
    """ Merge multiple wire networks into a single WireNetwork object.
    """
    vertices = [w.vertices for w in wire_networks]
    num_vertices = [w.num_vertices for w in wire_networks]
    offsets = np.cumsum([0] + num_vertices)

    edges = [w.edges.reshape((-1, 2)) + offsets[i]
            for i,w in enumerate(wire_networks)]

    vertices = np.vstack(vertices)
    edges = np.vstack(edges)
    return form_wires(vertices, edges)

