import PyMesh
try:
    from PyMesh import Gmpz
    from PyMesh import Gmpq

    Gmpz.__doc__ = """ Arbitrary precision integer class from GMP_.

    The following operations are supported:

        * Addition/subtraction:

            >>> a = pymesh.Gmpz(1)
            >>> b = pymesh.Gmpz(2)
            >>> a + b
            3
            >>> a + 10
            11
            >>> 10 + b
            12
            >>> a += b
            >>> a
            3
            >>> a += 5
            >>> a
            8
            >>> b -= 10
            >>> b
            -8

        * Multiplication/division:

            >>> a = pymesh.Gmpz(1)
            >>> b = pymesh.Gmpz(2)
            >>> a * b
            2
            >>> b / a
            2
            >>> a / b
            0
            >>> 10 / b
            5
            >>> a *= 6
            >>> a
            6
            >>> a /= 3
            >>> a
            2

        * Bit operations:

            >>> a = pymesh.Gmpz(1)
            >>> b = a << 2
            >>> b
            4
            >>> b <<=100
            >>> b
            5070602400912917605986812821504
            >>> b = pymesh.Gmpz(6)
            >>> b & a
            0
            >>> b | a
            7

        * size query:

            >>> a = pymesh.Gmpz(1)
            >>> a.bit_size
            1L
            >>> a <<= 100
            >>> a.bit_size
            101L

        * Convert to double:

            >>> a = pymesh.Gmpz(1)
            >>> type(a)
            <class 'PyMesh.Gmpz'>
            >>> x = a.to_double()
            >>> x
            1.0
            >>> type(x)
            <type 'float'>

    .. _GMP: https://gmplib.org/
    """

    Gmpq.__doc__ = """ Arbitrary precision fraction class from GMP_.

    Example:

        >>> a = pymesh.Gmpq(0.5)
        >>> a
        1/2
        >>> a.numerator
        1
        >>> a.denominator
        2
        >>> a /= 3
        >>> a
        1/6
        >>> a += a
        >>> a
        1/3
        >>> x = a.to_double()
        >>> x
        0.3333333333333333

    .. _GMP: https://gmplib.org/
    """
except:
    class Gmpz:
        def __init__(self):
            raise NotImplementedError("Gmpz feature is disabled")

    class Gmpq:
        def __init__(self):
            raise NotImplementedError("Gmpq feature is disabled")

