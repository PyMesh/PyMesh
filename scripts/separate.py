#!/usr/bin/env python
import argparse
import os.path
from pymesh import load_mesh, save_mesh
from pymesh import separate_mesh

def parse_args():
    parser = argparse.ArgumentParser(description="Wrapper around mesh_cc");
    parser.add_argument("--force", help="force separation", action="store_true");
    parser.add_argument("--connectivity-type", "-c",
            choices=["auto", "vertex", "face", "voxel"],
            help="the meaning of connected", default="auto");
    parser.add_argument("mesh_in", help="input mesh");
    parser.add_argument("mesh_out", help="output mesh");
    args = parser.parse_args();
    return args;

def main():
    args = parse_args();

    mesh = load_mesh(args.mesh_in);
    comps = separate_mesh(mesh, args.connectivity_type);

    basename, ext = os.path.splitext(args.mesh_out);
    for i,comp in enumerate(comps):
        filename = "{}_cc{}{}".format(basename, i, ext);
        save_mesh(filename, comp);

if __name__ == "__main__":
    main();
