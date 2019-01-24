#!/usr/bin/env python

"""
Compute the convex hull of a given mesh.
"""

import argparse
import pymesh
import numpy as np

def parse_args():
    parser = argparse.ArgumentParser(__doc__);
    parser.add_argument("--engine", default="cgal",
            choices=["cgal"]);
    parser.add_argument("--with-timing", help="output timing info",
            action="store_true");
    parser.add_argument("input_file");
    parser.add_argument("output_file");
    return parser.parse_args();

def main():
    """
    TODO: 
    1 . implement svg or other filebased import
    2 . implement exporting skeleton as svg  file
    args = parse_args();
    """

    contor = np.array( [[100.0,100.0],
                        [300.0,100.0],
                        [300.0,300.0],
                        [100.0,300.0]] );
    
    skeleton, running_time = pymesh.skeleton(contor, "cgal", True);
    print("Running time: {}s".format(running_time));

    print("Skeleton : {}".format(skeleton));
    
if __name__ == "__main__":
    main();