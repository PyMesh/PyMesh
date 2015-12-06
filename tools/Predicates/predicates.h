#pragma once

void exactinit();
double orient2d(double pa[2], double pb[2], double pc[2]);
double orient2dexact(double pa[2], double pb[2], double pc[2]);
double orient3d(double pa[3], double pb[3], double pc[3], double pd[3]);
double orient3dexact(double pa[3], double pb[3], double pc[3], double pd[3]);
double incircle(double pa[2], double pb[2], double pc[2], double pd[2]);
double incircleexact(double pa[2], double pb[2], double pc[2], double pd[2]);
double insphere(double pa[3], double pb[3], double pc[3], double pd[3], double pe[3]);
double insphereexact(double pa[3], double pb[3], double pc[3], double pd[3], double pe[3]);
