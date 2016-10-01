import PySparseSolver

class SparseSolver(object):
    """ Solver for sparse linear systems.

    Usage:

    >>> solver = pymesh.SparseSolver(matrix, engine="LLT");
    >>> solver.factorize()
    >>> sol = solver.solve(rhs)
    """
    def __init__(self, matrix):
        self.matrix = PySparseSolver.csc2ZSparseMatrix(matrix.tocsc());

    def factorize(self, engine):
        """ Factorize the input matrix.
        """
        self.__solver = PySparseSolver.SparseSolverFactory(engine).create();
        self.__solver.analyze_pattern(self.matrix);
        self.__solver.factorize(self.matrix);

    def solve(self, rhs):
        """ Solve the linear system.

        Args:
            rhs (:class"`numpy.ndarray`): The right hand side.

        Returns: The solution x for :math:`A x = rhs`.
        """
        try:
            solver = self.__solver;
        except AttributeError:
            raise RuntimeError("Need to factorize before solve!");
        return solver.solve(rhs);
