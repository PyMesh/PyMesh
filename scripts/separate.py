#!/usr/bin/env python

"""
Separate/highlight the disconnected components of the input mesh.

Connectivity can be defined as sharing a vertex/edge or face using
the --connectivity-type option.
"""

import argparse
import os.path
from pymesh import load_mesh, save_mesh
from pymesh import separate_mesh, merge_meshes

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("--force", help="force separation", action="store_true");
    parser.add_argument("--connectivity-type", "-c",
            choices=["auto", "vertex", "face", "voxel"],
            help="the meaning of connected", default="auto");
    parser.add_argument("--highlight", action="store_true",
            help="Highlight disconnected components");
    parser.add_argument("mesh_in", help="input mesh");
    parser.add_argument("mesh_out", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();

    mesh = load_mesh(args.mesh_in);
    comps = separate_mesh(mesh, args.connectivity_type);

    if args.highlight:
        mesh = merge_meshes(comps);
        save_mesh(args.mesh_out, mesh, *mesh.get_attribute_names());
    else:
        basename, ext = os.path.splitext(args.mesh_out);
        for i,comp in enumerate(comps):
            filename = "{}_cc{}{}".format(basename, i, ext);
            save_mesh(filename, comp);

if __name__ == "__main__":
    main();
