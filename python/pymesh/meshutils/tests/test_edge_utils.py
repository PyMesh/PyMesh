from pymesh.TestCase import TestCase
from pymesh import chain_edges

import numpy as np

class EdgeUtilsTest(TestCase):
    def test_single_edge(self):
        E = np.array([
            [0, 1]
            ])
        chains = chain_edges(E)
        self.assertEqual(1, len(chains))
        self.assert_array_equal([0, 1], chains[0])

    def test_two_edges(self):
        E = np.array([
            [0, 1],
            [1, 2]
            ])
        chains = chain_edges(E)
        self.assertEqual(1, len(chains))
        self.assert_array_equal([0, 1, 2], chains[0])

    def test_loop(self):
        E = np.array([
            [0, 1],
            [1, 2],
            [2, 0]
            ])
        chains = chain_edges(E)
        self.assertEqual(1, len(chains))

        chain = chains[0].ravel()
        start_idx = np.where(chain == 0)[0]
        chain = np.roll(chain, -start_idx)
        self.assert_array_equal([0, 1, 2], chain)

    def test_double_chain(self):
        E = np.array([
            [0, 1],
            [1, 2],
            [2, 0],
            [3, 4]
            ])
        chains = chain_edges(E)
        self.assertEqual(2, len(chains))

        for chain in chains:
            if len(chain) == 2:
                self.assert_array_equal([3, 4], chain)
            elif len(chain) == 3:
                start_idx = np.where(chain == 0)[0]
                chain = np.roll(chain, -start_idx)
                self.assert_array_equal([0, 1, 2], chain)
            else:
                self.fail("Unexpected chain size: {}".format(len(chain)))

    def test_non_simple_loop(self):
        E = np.array([
            [0, 1],
            [1, 2],
            [2, 0],
            [0, 3]
            ])
        self.assertRaises(RuntimeError, chain_edges, E)
