#pragma once

void exactinit();
double orient2d(double* pa, double* pb, double* pc);
double orient2dexact(double* pa, double* pb, double* pc);
double orient3d(double* pa, double* pb, double* pc, double* pd);
double orient3dexact(double* pa, double* pb, double* pc, double* pd);
double incircle(double* pa, double* pb, double* pc, double* pd);
double incircleexact(double* pa, double* pb, double* pc, double* pd);
double insphere(double* pa, double* pb, double* pc, double* pd, double* pe);
double insphereexact(double* pa, double* pb, double* pc, double* pd, double* pe);
