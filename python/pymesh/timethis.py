#!/usr/bin/env python
from time import clock;
from time import time;
import functools

class timethis(object):
    """ This class should be used as decorator for timing methods.
    Example:
        @timethis
        def foo(x, y):
            ...
    or
        @timethis_print
        def bar(self, a, b, c):
            ...
    """
    def __init__(self, f):
        self.f = f;

    def __call__(self, *args, **kwargs):
        self.tik();
        r = self.f(*args, **kwargs);
        self.tok();
        return r;

    def __get__(self, obj, objtype):
        """Support instance methods."""
        return functools.partial(self.__call__, obj)

    def tik(self):
        f_name = self.__get_name();
        if f_name not in self.__tiks:
            self.tiks[f_name] = time();
        else:
            raise RuntimeError("Overwriting previous tik for '{}'".format(f_name));

    def tok(self):
        finish = time();
        f_name = self.__get_name();
        start = self.tiks.pop(f_name, None);
        if start is None:
            raise RuntimeError("No tik before tok for '{}'".format(f_name));

        duration = finish - start;
        self.hist[f_name]  = self.hist.get(f_name, 0) + duration;
        self.count[f_name] = self.count.get(f_name, 0) + 1;
        return duration;

    def __get_name(self):
        func_name = self.f.__name__;
        module_name = self.f.__module__;
        return "{}.{}".format(module_name, func_name);

    @classmethod
    def summarize(cls):
        if len(cls.__hist) == 0: return;
        separator = "-"*67;
        format_string = "| {0:40.39} | {1:12.7} | {2:5} |";
        print(separator);
        print(format_string.format("Label", "Time", "Count"));
        print(separator);
        for f_name in cls.__hist.keys():
            print(format_string.format(
                f_name,
                cls.__hist[f_name],
                cls.__count[f_name]));
        print(separator);

        if len(cls.__tiks) > 0:
            print("==== orphans tiks ====");
            print(cls.__tiks.keys());

    @property
    def tiks(self):
        return self.__class__.__tiks;

    @property
    def hist(self):
        return self.__class__.__hist;

    @property
    def count(self):
        return self.__class__.__count;

    # Static variables
    __tiks = {};
    __hist = {};
    __count = {};

class timethis_print(timethis):
    def tok(self):
        duration = super(self.__class__, self).tok();
        print("Time of {:<10}: {:6.3}".format(self.f.__name__, duration));

@timethis
def test(x):
    print("x = {}".format(x));

class A(object):
    @timethis
    def test2(self, x):
        return x+1;

if __name__ == "__main__":
    a = A();
    for i in range(10):
        test(i);
        print(a.test2(i));
    timethis.summarize();

