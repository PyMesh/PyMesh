import math
import numpy as np
from numpy.linalg import norm
from ..meshio import form_mesh
from ..misc import Quaternion

def generate_tube(p0, p1, r0_out, r1_out, r0_in, r1_in, num_segments=16,
        with_quad=False):
    """ Generate generalized tube (i.e. cylinder with an axial hole).

    Args:
        p0 (``np.ndarray``): Bottom center.
        p1 (``np.ndarray``): Top center.
        r0_out (``float``): Bottom outer radius.
        r1_out (``float``): Top outer radius.
        r0_in (``float``): Bottom inner radius.
        r1_in (``float``): Top inner radius.
        num_segments (``int``): Number of segments to a discrete circle consists.
        with_quad (``bool``): Output a quad mesh instead.

    Returns:
        A generalized tube :py:class:`Mesh`.
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
    N = num_segments

    angles = [2*math.pi*i/float(N) for i in range(N)]
    rim = np.array([[math.cos(theta), math.sin(theta), 0.0]
        for theta in angles])
    rot = Quaternion.fromData(Z, axis).to_matrix()

    bottom_outer_rim = np.dot(rot, rim.T).T * r0_out + p0
    bottom_inner_rim = np.dot(rot, rim.T).T * r0_in + p0
    top_outer_rim = np.dot(rot, rim.T).T * r1_out + p1
    top_inner_rim = np.dot(rot, rim.T).T * r1_in + p1

    vertices = np.vstack([
        bottom_outer_rim,
        bottom_inner_rim,
        top_outer_rim,
        top_inner_rim])

    if with_quad:
        top = np.array([
            [2*N+i, 2*N+(i+1)%N, 3*N+(i+1)%N, 3*N+i]
            for i in range(N)])
        bottom = np.array([
            [(i+1)%N,   i, N+i, N+(i+1)%N]
            for i in range(N)])
        inner = np.array([
            [3*N+i, 3*N+(i+1)%N, N+(i+1)%N, N+i]
            for i in range(N)])
        outer = np.array([
            [    i,     (i+1)%N, 2*N+(i+1)%N, 2*N+i]
            for i in range(N)])
        faces = np.vstack([top, bottom, inner, outer])
    else:
        top = np.array([
            [ [2*N+i, 2*N+(i+1)%N, 3*N+i],
              [3*N+i, 2*N+(i+1)%N, 3*N+(i+1)%N]
            ] for i in range(N)])
        bottom = np.array([
            [ [(i+1)%N,   i, N+i],
              [(i+1)%N, N+i, N+(i+1)%N]
            ] for i in range(N)])
        inner = np.array([
            [ [3*N+i, 3*N+(i+1)%N, N+i],
              [  N+i, 3*N+(i+1)%N, N+(i+1)%N]
            ] for i in range(N)])
        outer = np.array([
            [ [    i,     (i+1)%N, 2*N+i],
              [2*N+i,     (i+1)%N, 2*N+(i+1)%N]
            ] for i in range(N)])

        faces = np.vstack([
            top.reshape((-1, 3)),
            bottom.reshape((-1, 3)),
            inner.reshape((-1, 3)),
            outer.reshape((-1, 3)) ])
    return form_mesh(vertices, faces)
