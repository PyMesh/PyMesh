import unittest
import PyMeshSetting
import PySwigTests

import numpy as np
from numpy.random import randn

class FunctionsTest(unittest.TestCase):
    def test_output(self):
        result = PySwigTests.output("abc");
        self.assertEqual("<abc>", result);

    def test_x2(self):
        data = randn(1e3);
        result = PySwigTests.x2(data);
        self.assertTrue(np.all(data * 2 == result));

    def test_x3(self):
        data = [1.0, 1.1];
        result = PySwigTests.x3(data);
        self.assertListEqual([x*3 for x in data], list(result));
