#!/usr/bin/env python

"""
Create bbox of a given mesh.
"""

import argparse
import numpy as np

import pymesh

def parse_args():
    parser = argparse.ArgumentParser(description=__doc__);
    parser.add_argument("input_mesh");
    parser.add_argument("output_mesh");
    return parser.parse_args();

def main():
    args = parse_args();
    mesh = pymesh.load_mesh(args.input_mesh);
    bbox = mesh.bbox;
    bbox_mesh = pymesh.generate_box_mesh(bbox[0], bbox[1], 1);
    pymesh.save_mesh(args.output_mesh, bbox_mesh);

if __name__ == "__main__":
    main();
