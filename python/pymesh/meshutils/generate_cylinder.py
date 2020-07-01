import math
import numpy as np
from numpy.linalg import norm
from ..meshio import form_mesh
from ..misc import Quaternion

def generate_cylinder(p0, p1, r0, r1, num_segments=16):
    """ Generate cylinder (or `conical frustum`_ to be precise).

    Args:
        p0 (``np.ndarray``): Bottom center.
        p1 (``np.ndarray``): Top center.
        r0 (``float``): Bottom radius.
        r1 (``float``): Top radius.
        num_segments (``int``): Number of segments of discrete circle.

    Returns:
        The cylinder :py:class:`Mesh`.

    .. _conical frustum: http://mathworld.wolfram.com/ConicalFrustum.html
    """

    assert(len(p0) == 3)
    assert(len(p1) == 3)
    Z = np.array([0, 0, 1], dtype=float)
    p0 = np.array(p0, dtype=float)
    p1 = np.array(p1, dtype=float)
    axis = p1 - p0
    l = norm(axis)
    if l <= 1e-12:
        axis=Z

    angles = [2*math.pi*i/float(num_segments) for i in range(num_segments)]
    rim = np.array([[math.cos(theta), math.sin(theta), 0.0]
        for theta in angles])
    rot = Quaternion.fromData(Z, axis).to_matrix()

    bottom_rim = np.dot(rot, rim.T).T * r0 + p0
    top_rim = np.dot(rot, rim.T).T * r1 + p1

    vertices = np.vstack([ [p0, p1], bottom_rim, top_rim ])

    bottom_fan = np.array([
        [0, (i+1)%num_segments+2, i+2]
        for i in range(num_segments) ], dtype=int)

    top_fan = np.array([
        [1, i+num_segments+2, (i+1)%num_segments+num_segments+2]
        for i in range(num_segments) ], dtype=int)

    side = np.array([
        [[2+i, 2+(i+1)%num_segments, 2+i+num_segments],
         [2+i+num_segments, 2+(i+1)%num_segments, 2+(i+1)%num_segments+num_segments] ]
        for i in range(num_segments) ], dtype=int)
    side = side.reshape((-1, 3), order="C")

    faces = np.vstack([bottom_fan, top_fan, side])
    return form_mesh(vertices, faces)
