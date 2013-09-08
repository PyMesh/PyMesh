#ifndef WINDING_NUMBER_H
#define WINDING_NUMBER_H

// Minimum number of iterms per openmp thread
#ifndef WINDING_NUMBER_OMP_MIN_VALUE
#  define WINDING_NUMBER_OMP_MIN_VALUE 1000
#endif
// WINDING_NUMBER Compute the sum of solid angles of a triangle/tetrahedron
// described by points (vectors) V
//
// Templates:
//   dim  dimension of input
// Inputs:
//  V  n by 3 list of vertex positions
//  n  number of mesh vertices
//  F  #F by 3 list of triangle indices, minimum index is 0
//  m  number of faces
//  O  no by 3 list of origin positions
//  no  number of origins
// Outputs:
//  S  no by 1 list of winding numbers
//
// 3d
template <typename DerivedF>
void winding_number_3(
  const double * V,
  const int n,
  const DerivedF * F,
  const int m,
  const double * O,
  const int no,
  double * S);
// Only one evaluation origin
template <typename DerivedF>
void winding_number_3(
  const double * V,
  const int n,
  const DerivedF * F,
  const int m,
  const double * O,
  double * S);

// 2d
template <typename DerivedF>
void winding_number_2(
  const double * V,
  const int n,
  const DerivedF * F,
  const int m,
  const double * O,
  const int no,
  double * S);

#endif
