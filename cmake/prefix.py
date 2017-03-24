import sys
"""
A unnecessarily complex script to locate python library location.

Normally, python library is stored under the `sys.prefix`.

However, when `virtualenv` or `venv` is used, `sys.prefix` got changed to the
virtual environment setting, which no longer contains python library.  Instead,
a new variable `sys.real_prefix` is defined to store the **real** prefix.

For python (>=3.3), `sys.base_prefix` will always point at the true prefix
regardless if `virtualenv` or `venv` is used.  However, `sys.base_prefix` is
not defined for python (<3.3).

Therefore, no single variable works all the time, and thus need for the
if-elif-else statement...
"""

if hasattr(sys, "real_prefix"):
    # In virtualenv or venv
    print(sys.real_prefix);
elif hasattr(sys, "base_prefix"):
    # Not in virtualenv or env, but has python >= 3.3
    print(sys.base_prefix);
else:
    # Not in virtualenv or env and python < 3.3, the default prefix should work.
    print(sys.prefix);
