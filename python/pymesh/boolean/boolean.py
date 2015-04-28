#!/usr/bin/env python

import PyBoolean
import numpy as np
from ..meshio import form_mesh

def auto_select_engine(dim):
    if dim == 2:
        engine = "clipper";
    elif dim == 3:
        engine = "cork";
    else:
        raise NotImplementedError("Dimension {} is not supported".format(dim));
    return engine;

def boolean(mesh_1, mesh_2, operation, engine="auto"):
    """ Perform boolean operations on input meshes.

    Args:
        mesh_1 (:class:`Mesh`): The first input mesh, :math:`M_1`.
        mesh_2 (:class:`Mesh`): The second input mesh, :math:`M_2`.
        operation (``string``): The name of the operation.  Valid choices are:

            * ``intersection``: :math:`M_1 \cap M_2`
            * ``union``: :math:`M_1 \cup M_2`
            * ``difference``: :math:`M_1 \setminus M_2`
            * ``symmetric_difference``: :math:`(M_1 \setminus M_2) \cup
              (M_2 \setminus M_1)`

        engine (``string``): (optional) Boolean engine name.  Valid engines include:

            * ``auto``: Using the default boolean engine
              (``cork`` for 3D and ``clipper`` for 2D).  This is the default.
            * ``cork``: `Cork 3D boolean libary
              <https://github.com/gilbo/cork>`_
            * ``cgal``: `CGAL 3D boolean operations on Nef Polyhedra
              <http://doc.cgal.org/latest/Nef_3/index.html>`_
            * ``igl``: `libigl's 3D boolean support
              <https://github.com/libigl/libigl>`_
            * ``clipper``: `Clipper 2D boolean library
              <http://www.angusj.com/delphi/clipper.php>`_

    Returns:
        2 values are returned.

            * ``output_mesh`` (:class:`Mesh`): output triangular mesh.
            * ``infomation`` (:class:`dict`): additional informations.

    """
    assert(mesh_1.dim == mesh_2.dim);
    assert(mesh_1.vertex_per_face == 3);
    assert(mesh_2.vertex_per_face == 3);
    dim = mesh_1.dim;

    if engine == "auto":
        auto_select_engine(dim);

    engine = PyBoolean.BooleanEngine.create(engine);
    engine.set_mesh_1(mesh_1.vertices, mesh_1.faces);
    engine.set_mesh_2(mesh_2.vertices, mesh_2.faces);

    if (operation == "intersection"):
        engine.compute_intersection();
    elif (operation == "union"):
        engine.compute_union();
    elif (operation == "difference"):
        engine.compute_difference();
    elif (operation == "symmetric_difference"):
        engine.compute_symmetric_difference();
    else:
        raise NotImplementedError(
                "Unsupported operations: {}".format(operation));

    output_mesh = form_mesh(engine.get_vertices(), engine.get_faces());
    info = {};
    return output_mesh, info;

def intersection(mesh_1, mesh_2, engine="auto"):
    """ Wrapper function of :meth:`pymesh.boolean.boolean`
    with operation = "intersection".
    """
    return boolean(mesh_1, mesh_2, "intersection", engine);

def union(mesh_1, mesh_2, engine="auto"):
    """ Wrapper function of :meth:`pymesh.boolean.boolean`
    with operation = "union".
    """
    return boolean(mesh_1, mesh_2, "union", engine);

def difference(mesh_1, mesh_2, engine="auto"):
    """ Wrapper function of :meth:`pymesh.boolean.boolean`
    with operation = "difference".
    """
    return boolean(mesh_1, mesh_2, "difference", engine);

def symmetric_difference(mesh_1, mesh_2, engine="auto"):
    """ Wrapper function of :meth:`pymesh.boolean.boolean`
    with operation = "symmetric_difference".
    """
    return boolean(mesh_1, mesh_2, "symmetric_difference", engine);
