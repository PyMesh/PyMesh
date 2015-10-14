Triangle Compilation Note
=========================

This note only apply to Linux environment.  It has been reported that linking
with dynamic triangle library (libtriangle.so) causes crashes and failed unit
tests.  The easiest fix is to link with static triangle library.  Unfortunately,
the default triangle make file would only generate ``triangle.o`` file.  To
generated the static library, I recommend::

    ar rcs libtriangle.a triangle.o

