%{
#include "TriangleMetric.h"
%}
%ignore TriangleMetric::update(const Point& p1, const Point& p2, const Point& p3, const Point& p4, const Point& p5, const Point& p6);
%include TriangleMetric.h
