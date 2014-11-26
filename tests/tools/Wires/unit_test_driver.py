#!/usr/bin/env python

import os
import os.path
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

from WireNetwork.WireNetworkTest import WireNetworkTest
#from Inflator.InflatorEngineTest import InflatorEngineTest
from Tiler.WireTilerTest import WireTilerTest

if __name__ == "__main__":
    unittest.main();
