#!/usr/bin/env python

import numpy as np
import os
import os.path
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

from PySwigEigen import CopyTest

class CopyArrayTest(unittest.TestCase):
    def setUp(self):
        self.int_array = np.arange(0, 10, dtype=int);
        self.float_array = np.arange(0, 10, 0.5, dtype=float);

        self.copy_obj = CopyTest();

    def test_float_array(self):
        self.copy_obj.set_f_data(self.float_array);
        data = self.copy_obj.get_f_data();
        self.assertItemsEqual(self.float_array, data);

    def test_int_array(self):
        self.copy_obj.set_i_data(self.int_array);
        data = self.copy_obj.get_i_data();
        self.assertItemsEqual(self.int_array, data);

    def test_update_f_data(self):
        self.copy_obj.set_f_data(self.float_array);
        data = self.copy_obj.get_f_data();
        data[0] = -1;
        self.assertEqual(-1, self.copy_obj.get_f_data()[0]);

    #def test_const_f_data(self):
    #    self.copy_obj.set_f_data(self.float_array);
    #    data = self.copy_obj.get_f_data();
    #    print(data.flags)
    #    data[0] = -1;

