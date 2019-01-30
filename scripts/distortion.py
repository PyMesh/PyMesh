#!/usr/bin/env python

"""
Compute distortion of each element.
"""

import argparse
import pymesh
import numpy as np
import numpy.linalg
import logging
import csv

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--log", type=str, help="logging level",
            choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            default="WARNING");
    parser.add_argument("--csv", type=str, help="output csv_file");
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def compute_distortion_energies_3D(mesh):
    if mesh.num_voxels > 0 and mesh.vertex_per_voxel != 4:
        raise RuntimeError("Only tet mesh is supported for distortion computation");

    regular_tet = pymesh.generate_regular_tetrahedron();
    assembler = pymesh.Assembler(regular_tet);
    G = assembler.assemble("gradient");

    vertices = mesh.vertices;
    tets = mesh.voxels;
    Js = [ G * vertices[tet] for tet in tets ];

    J_F = np.array([np.trace(np.dot(J.T, J)) for J in Js]);
    J_det = np.array([numpy.linalg.det(J) for J in Js]);
    invert_J = lambda args: np.full((3,3), np.inf) if args[1] == 0 else numpy.linalg.inv(args[0])
    J_inv = map(invert_J, zip(Js, J_det));
    J_inv_F = np.array([np.trace(np.dot(Ji.T, Ji)) for Ji in J_inv]);

    conformal_amips = np.divide(J_F, np.cbrt(np.square(J_det)));
    finite_conformal_amips = np.isfinite(conformal_amips);
    symmetric_dirichlet = J_F + J_inv_F;
    finite_symmetric_dirichlet = np.isfinite(symmetric_dirichlet);
    orientations = pymesh.get_tet_orientations(mesh);
    orientations[orientations > 0] = 1;
    orientations[orientations < 0] = -1;

    num_degenerate_tets = np.count_nonzero(orientations==0);
    num_inverted_tets = np.count_nonzero(orientations<0);
    num_nonfinite_amips = np.count_nonzero(np.logical_not(finite_conformal_amips));
    num_nonfinite_dirichlet =\
            np.count_nonzero(np.logical_not(finite_symmetric_dirichlet));
    logger = logging.getLogger("Distorsion");
    if num_degenerate_tets > 0:
        logger.warn("degenerate tets: {}".format(num_degenerate_tets));
    if num_inverted_tets > 0:
        logger.warn("inverted tets: {}".format(num_inverted_tets));
    if num_nonfinite_amips > 0:
        logger.warn("Non-finite conformal AMIPS: {}".format(
            num_nonfinite_amips));
    if num_nonfinite_dirichlet > 0:
        logger.warn("Non-finite symmetric Dirichlet: {}".format(
            num_nonfinite_dirichlet));

    mesh.add_attribute("conformal_AMIPS");
    mesh.set_attribute("conformal_AMIPS", conformal_amips);
    mesh.add_attribute("finite_conformal_AMIPS");
    mesh.set_attribute("finite_conformal_AMIPS", finite_conformal_amips);
    mesh.add_attribute("symmetric_Dirichlet");
    mesh.set_attribute("symmetric_Dirichlet", symmetric_dirichlet);
    mesh.add_attribute("finite_symmetric_Dirichlet");
    mesh.set_attribute("finite_symmetric_Dirichlet", finite_symmetric_dirichlet);
    mesh.add_attribute("orientations");
    mesh.set_attribute("orientations", orientations);

def compute_distortion_energies_2D(mesh):
    assert(mesh.dim == 2);
    regular_tri = pymesh.generate_equilateral_triangle();
    assembler = pymesh.Assembler(regular_tri);
    G = assembler.assemble("gradient");

    vertices = mesh.vertices;
    tris = mesh.faces;
    Js = [ G * vertices[tri] for tri in tris];

    J_F = np.array([np.trace(np.dot(J.T, J)) for J in Js]);
    J_det = np.array([numpy.linalg.det(J) for J in Js]);
    invert_J = lambda args: np.full((3,3), np.inf) if args[1] == 0 else numpy.linalg.inv(args[0])
    J_inv = map(invert_J, zip(Js, J_det));
    J_inv_F = np.array([np.trace(np.dot(Ji.T, Ji)) for Ji in J_inv]);

    conformal_amips = np.divide(J_F, J_det);
    finite_conformal_amips = np.isfinite(conformal_amips);
    symmetric_dirichlet = J_F + J_inv_F;
    finite_symmetric_dirichlet = np.isfinite(symmetric_dirichlet);

    orientations = np.array([pymesh.orient_2D(
        vertices[t[0]], vertices[t[1]], vertices[t[2]])
        for t in tris]);
    orientations[orientations > 0] = 1;
    orientations[orientations < 0] = -1;

    num_degenerate_tets = np.count_nonzero(orientations==0);
    num_inverted_tets = np.count_nonzero(orientations<0);
    num_nonfinite_amips = np.count_nonzero(np.logical_not(finite_conformal_amips));
    num_nonfinite_dirichlet =\
            np.count_nonzero(np.logical_not(finite_symmetric_dirichlet));
    logger = logging.getLogger("Distorsion");
    if num_degenerate_tets > 0:
        logger.warn("degenerate tets: {}".format(num_degenerate_tets));
    if num_inverted_tets > 0:
        logger.warn("inverted tets: {}".format(num_inverted_tets));
    if num_nonfinite_amips > 0:
        logger.warn("Non-finite conformal AMIPS: {}".format(
            num_nonfinite_amips));
    if num_nonfinite_dirichlet > 0:
        logger.warn("Non-finite symmetric Dirichlet: {}".format(
            num_nonfinite_dirichlet));

    mesh.add_attribute("conformal_AMIPS");
    mesh.set_attribute("conformal_AMIPS", conformal_amips);
    mesh.add_attribute("finite_conformal_AMIPS");
    mesh.set_attribute("finite_conformal_AMIPS", finite_conformal_amips);
    mesh.add_attribute("symmetric_Dirichlet");
    mesh.set_attribute("symmetric_Dirichlet", symmetric_dirichlet);
    mesh.add_attribute("finite_symmetric_Dirichlet");
    mesh.set_attribute("finite_symmetric_Dirichlet", finite_symmetric_dirichlet);
    mesh.add_attribute("orientations");
    mesh.set_attribute("orientations", orientations);

def compute_grad_upper_bound(mesh):
    """ The measure introduced in the following paper:

    Shewchuk, Jonathan. "What is a good linear finite element? interpolation,
    conditioning, anisotropy, and quality measures (preprint)." University of
    California at Berkeley 73 (2002).
    """
    area = lambda p0, p1, p2: 0.5 * numpy.linalg.norm(np.cross(p1-p0, p2-p0));
    measure = np.zeros(mesh.num_voxels);
    mesh.add_attribute("voxel_volume");
    vols = mesh.get_attribute("voxel_volume").ravel();
    vertices = mesh.vertices;
    tets = mesh.voxels;
    for i in range(mesh.num_voxels):
        tet = tets[i];
        p = vertices[tet];
        A0 = area(p[1], p[2], p[3]);
        A1 = area(p[0], p[2], p[3]);
        A2 = area(p[0], p[1], p[3]);
        A3 = area(p[0], p[1], p[2]);
        l01 = numpy.linalg.norm(p[0] - p[1]);
        l02 = numpy.linalg.norm(p[0] - p[2]);
        l03 = numpy.linalg.norm(p[0] - p[3]);
        l12 = numpy.linalg.norm(p[1] - p[2]);
        l13 = numpy.linalg.norm(p[1] - p[3]);
        l23 = numpy.linalg.norm(p[2] - p[3]);
        V = vols[i];
        measure[i] = ((A0*A1*l01*l01 + A0*A2*l02*l02 + A0*A3*l03*l03
                + A1*A2*l12*l12 + A1*A3*l13*l13 + A2*A3*l23*l23) / (6.0 * V)
                + np.amax([A0*l01, A0*l02, A0*l03,
                    A1*l01, A1*l12, A1*l13,
                    A2*l02, A2*l12, A2*l23,
                    A3*l03, A3*l13, A3*l23])) / (A0+A1+A2+A3) /\
                            np.amax([l01, l02, l03, l12, l13, l23]);
    mesh.add_attribute("grad_bound");
    mesh.set_attribute("grad_bound", measure);

def compute_tet_quality_measures(mesh):
    mesh.add_attribute("voxel_inradius");
    mesh.add_attribute("voxel_circumradius");
    inradius = mesh.get_attribute("voxel_inradius");
    circumradius = mesh.get_attribute("voxel_circumradius");
    radius_ratio = np.divide(inradius, circumradius);
    mesh.add_attribute("radius_ratio");
    mesh.set_attribute("radius_ratio", radius_ratio);
    mesh.add_attribute("voxel_dihedral_angle");
    dihedral_angle = mesh.get_voxel_attribute("voxel_dihedral_angle");
    if mesh.num_voxels == 0:
        min_dihedral_angle = np.zeros(0);
        max_dihedral_angle = np.zeros(0);
    else:
        min_dihedral_angle = np.amin(dihedral_angle, axis=1);
        max_dihedral_angle = np.amax(dihedral_angle, axis=1);
    mesh.add_attribute("voxel_min_dihedral_angle");
    mesh.set_attribute("voxel_min_dihedral_angle", min_dihedral_angle);
    mesh.add_attribute("voxel_max_dihedral_angle");
    mesh.set_attribute("voxel_max_dihedral_angle", max_dihedral_angle);
    mesh.add_attribute("voxel_radius_edge_ratio");
    mesh.add_attribute("voxel_edge_ratio");
    compute_grad_upper_bound(mesh);

def compute_tri_quality_measures(mesh):
    mesh.add_attribute("face_incircle_radius");
    mesh.add_attribute("face_circumradius");
    inradius = mesh.get_attribute("face_incircle_radius");
    circumradius = mesh.get_attribute("face_circumradius");
    radius_ratio = np.divide(inradius, circumradius);
    mesh.add_attribute("radius_ratio");
    mesh.set_attribute("radius_ratio", radius_ratio);
    mesh.add_attribute("face_radius_edge_ratio");
    mesh.add_attribute("face_edge_ratio");
    #compute_grad_upper_bound(mesh);

def output_to_csv_3D(mesh, csv_file):
    with open(csv_file, 'w') as fout:
        writer = csv.writer(fout);
        writer.writerow(["index", "edge_ratio", "radius_ratio",
            "min_dihedral_angle", "max_dihedral_angle", "radius_edge_ratio",
            "conformal_amips", "symmetric_dirichlet", "orientation", "grad_bound"]);

        index = np.arange(mesh.num_voxels);
        edge_ratio = mesh.get_attribute("voxel_edge_ratio");
        radius_ratio = mesh.get_attribute("radius_ratio");
        min_dihedral = mesh.get_attribute("voxel_min_dihedral_angle");
        max_dihedral = mesh.get_attribute("voxel_max_dihedral_angle");
        re_ratio = mesh.get_attribute("voxel_radius_edge_ratio");
        amips = mesh.get_attribute("conformal_AMIPS");
        dirichlet = mesh.get_attribute("symmetric_Dirichlet");
        orientation = mesh.get_attribute("orientations");
        grad_bound = mesh.get_attribute("grad_bound");

        for i in range(mesh.num_voxels):
            writer.writerow([i,
                edge_ratio[i],
                radius_ratio[i],
                min_dihedral[i],
                max_dihedral[i],
                re_ratio[i],
                amips[i],
                dirichlet[i],
                orientation[i],
                grad_bound[i]]);

def output_to_csv_2D(mesh, csv_file):
    with open(csv_file, 'w') as fout:
        writer = csv.writer(fout);
        writer.writerow(["index", "edge_ratio", "radius_ratio",
            "radius_edge_ratio",
            "conformal_amips", "symmetric_dirichlet", "orientation"]);

        index = np.arange(mesh.num_faces);
        edge_ratio = mesh.get_attribute("face_edge_ratio");
        radius_ratio = mesh.get_attribute("radius_ratio");
        re_ratio = mesh.get_attribute("face_radius_edge_ratio");
        amips = mesh.get_attribute("conformal_AMIPS");
        dirichlet = mesh.get_attribute("symmetric_Dirichlet");
        orientation = mesh.get_attribute("orientations");

        for i in range(mesh.num_faces):
            writer.writerow([i,
                edge_ratio[i],
                radius_ratio[i],
                re_ratio[i],
                amips[i],
                dirichlet[i],
                orientation[i], ]);

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh, drop_zero_dim=True);
    if mesh.num_voxels > 0 and mesh.vertex_per_voxel != 4:
        raise RuntimeError("Only tet mesh is supported for distortion computation");
    elif mesh.num_voxels == 0 and mesh.num_faces > 0 and mesh.vertex_per_face!=3:
        raise RuntimeError("Only triangle mesh is supported for distortion computation");

    numeric_level = getattr(logging, args.log, None);
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % loglevel);
    logging.basicConfig(level=numeric_level);

    if mesh.num_voxels > 0:
        compute_distortion_energies_3D(mesh);
        compute_tet_quality_measures(mesh);
        if args.csv != None:
            output_to_csv_3D(mesh, args.csv);
    else:
        compute_distortion_energies_2D(mesh);
        compute_tri_quality_measures(mesh);
        if args.csv != None:
            output_to_csv_2D(mesh, args.csv);

    pymesh.save_mesh(args.output_mesh, mesh, *mesh.attribute_names);

if __name__ == "__main__":
    main();
