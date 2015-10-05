#!/usr/bin/env python

"""
Highlight exactly degenerated faces.
"""

import argparse
import numpy as np
import pymesh

from submesh import extract_submesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--extract-region", help="exact problematic_region",
            default=None);
    parser.add_argument("--verbose", "-v", action="store_true",
            help="print degenerated face indices");
    parser.add_argument("input_mesh", help="input mesh");
    parser.add_argument("output_mesh", help="output mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);

    f_indices = pymesh.get_degenerated_faces(mesh);
    if (len(f_indices) == 0):
        print("mesh does not have any degenerated faces");
        return;

    v_indices = mesh.faces[f_indices].ravel();
    degenerated_faces = np.zeros(mesh.num_faces);
    degenerated_faces[f_indices] = 1.0;
    degenerated = np.zeros(mesh.num_vertices);
    degenerated[v_indices] = 1.0;
    mesh.add_attribute("degenerated_faces");
    mesh.set_attribute("degenerated_faces", degenerated_faces);
    mesh.add_attribute("degenerated");
    mesh.set_attribute("degenerated", degenerated);

    print("{} degenerated faces, consisting of {} vertices.".format(
        len(f_indices), np.count_nonzero(degenerated)));
    if args.verbose:
        print("Degenerated faces indices: {}".format(f_indices));

    pymesh.save_mesh(args.output_mesh, mesh, "degenerated", "degenerated_faces");

    if args.extract_region is not None:
        region = extract_submesh(mesh, f_indices, 0);
        pymesh.save_mesh(args.extract_region, region,
                *region.get_attribute_names());

if __name__ == "__main__":
    main();
