from PyMesh import SparseSolver
import PyMesh

class SparseSolver(PyMesh.SparseSolver):
    """ Linear solver for solving sparse linear systems.

    This class is a thin wrapper around `sparse solvers supported by Eigen
    <https://eigen.tuxfamily.org/dox/group__TopicSparseSystems.html>`_.

    The following direct solvers are supported:

    * ``LLT``: Wrapper of `Eigen::SimplicialLLT`_, SPD only.
    * ``LDLT``: Wrapper of `Eigen::SimplicialLDLT`_, SPD only.
    * ``SparseLU``: Wrapper of `Eigen::SparseLU`_.
    * ``SparseQR``: Wrapper of `Eigen::SparseQR`_.
    * ``UmfPackLU``: Wrapper of `Eigen::UmfPackLU`_.  (Require `SuiteSparse`_)
    * ``Cholmod``: Wrapper of `Eigen::CholmodSupernodalLLT`_. (Require `SuiteSparse`_)
    * ``PardisoLLT``: Wrapper of `Eigen::PardisoLLT`_. SPD only. (Require `Intel MKL`_)
    * ``PardisoLDLT``: Wrapper of `Eigen::PardisoLDLT`_. SPD only. (Require `Intel MKL`_)
    * ``PardisoLU``: Wrapper of `Eigen::PardisoLU`_. (Require `Intel MKL`_)

    The following iterative solvers are supported:

    * ``CG``: Wrapper of `Eigen::ConjugateGradient`_. SPD only.
    * ``LSCG``: Wrapper of `Eigen::LeastSquaresConjugateGradient`_.
    * ``BiCG``: Wrapper of `Eigen::BiCGSTAB`_.

    Attributes:

        supported_solvers (``list`` of ``str``): The list of supported solvers.

        tolerance (``float``): The residual error threshold for stopping
            iterative solvers.  Default is ``Eigen::NumTraits<Float>::epsilon()``.

        max_iterations (``int``):  The max iterations allowed for iterative
            solvers.  Default is twice the number of columns of the matrix.

    Example:

        For direct solvers:

        >>> M = scipy.sparse.eye(100) # Build matrix.
        >>> rhs = numpy.ones(100) # build right hand side.
        >>> solver = pymesh.SparseSolver.create("LDLT")
        >>> solver.compute(M)
        >>> x = solver.solve(rhs)

        For iterative solvers:

        >>> M = scipy.sparse.eye(100) # Build matrix.
        >>> rhs = numpy.ones(100) # build right hand side.
        >>> solver = pymesh.SparseSolver.create("CG")
        >>> solver.tolerance = 1e-10
        >>> solver.max_iterations = 50
        >>> solver.compute(M)
        >>> x = solver.solve(rhs)

    .. _`Eigen::SimplicialLLT`: https://eigen.tuxfamily.org/dox/classEigen_1_1SimplicialLLT.html
    .. _`Eigen::SimplicialLDLT`: https://eigen.tuxfamily.org/dox/classEigen_1_1SimplicialLDLT.html
    .. _`Eigen::SparseLU`: https://eigen.tuxfamily.org/dox/classEigen_1_1SparseLU.html
    .. _`Eigen::SparseQR`: https://eigen.tuxfamily.org/dox/classEigen_1_1SparseQR.html
    .. _`Eigen::UmfPackLU`: https://eigen.tuxfamily.org/dox/classEigen_1_1UmfPackLU.html
    .. _`Eigen::CholmodSupernodalLLT`: https://eigen.tuxfamily.org/dox/classEigen_1_1CholmodSupernodalLLT.html
    .. _`Eigen::PardisoLLT`: https://eigen.tuxfamily.org/dox/classEigen_1_1PardisoLLT.html
    .. _`Eigen::PardisoLDLT`: https://eigen.tuxfamily.org/dox/classEigen_1_1PardisoLDLT.html
    .. _`Eigen::PardisoLU`: https://eigen.tuxfamily.org/dox/classEigen_1_1PardisoLU.html
    .. _`Eigen::ConjugateGradient`: https://eigen.tuxfamily.org/dox/classEigen_1_1ConjugateGradient.html
    .. _`Eigen::LeastSquaresConjugateGradient`: https://eigen.tuxfamily.org/dox/classEigen_1_1LeastSquaresConjugateGradient.html
    .. _`Eigen::BiCGSTAB`: https://eigen.tuxfamily.org/dox/classEigen_1_1BiCGSTAB.html
    .. _`SuiteSparse`: http://faculty.cse.tamu.edu/davis/suitesparse.html
    .. _`Intel MKL`: https://software.intel.com/en-us/mkl
    """
    @property
    def supported_solvers(self):
        return self.get_supported_solvers()

    @classmethod
    def get_supported_solvers(cls):
        return PyMesh.SparseSolver.get_supported_solvers()
