%{
extern "C" {
#include <Predicates/predicates.h>
}
%}
%apply (double IN_ARRAY1[ANY]) {(double pa[3])}
%apply (double IN_ARRAY1[ANY]) {(double pb[3])}
%apply (double IN_ARRAY1[ANY]) {(double pc[3])}
%apply (double IN_ARRAY1[ANY]) {(double pd[3])}
%apply (double IN_ARRAY1[ANY]) {(double pe[3])}
%apply (double IN_ARRAY1[ANY]) {(double pa[2])}
%apply (double IN_ARRAY1[ANY]) {(double pb[2])}
%apply (double IN_ARRAY1[ANY]) {(double pc[2])}
%apply (double IN_ARRAY1[ANY]) {(double pd[2])}
%apply (double IN_ARRAY1[ANY]) {(double pe[2])}
%include "predicates.h"
