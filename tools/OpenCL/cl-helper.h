/*
 * Copyright (c) 2010, 2012 Andreas Kloeckner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */




#ifndef NYUHPC_CL_HELPER
#define NYUHPC_CL_HELPER

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

/* An error check macro for OpenCL.
 *
 * Usage:
 * CHECK_CL_ERROR(status_code_from_a_cl_operation, "function_name")
 *
 * It will abort with a message if an error occurred.
 */

#define CHECK_CL_ERROR(STATUS_CODE, WHAT) \
  if ((STATUS_CODE) != CL_SUCCESS) \
  { \
    fprintf(stderr, \
        "*** '%s' in '%s' on line %d failed with error '%s'.\n", \
        WHAT, __FILE__, __LINE__, \
        cl_error_to_str(STATUS_CODE)); \
    abort(); \
  }

/* A more automated error check macro for OpenCL, for use with clXxxx
 * functions that return status codes. (Not all of them do, notably 
 * clCreateXxx do not.)
 *
 * Usage:
 * CALL_CL_GUARDED(clFunction, (arg1, arg2));
 *
 * Note the slightly strange comma between the function name and the
 * argument list.
 */

#define CALL_CL_GUARDED(NAME, ARGLIST) \
  { \
    cl_int status_code; \
      status_code = NAME ARGLIST; \
    CHECK_CL_ERROR(status_code, #NAME); \
  }

/* An error check macro for Unix system functions. If "COND" is true, then the
 * last system error ("errno") is printed along with MSG, which is supposed to
 * be a string describing what you were doing.
 *
 * Example:
 * CHECK_SYS_ERROR(dave != 0, "opening hatch");
 */
#define CHECK_SYS_ERROR(COND, MSG) \
  if (COND) \
  { \
    perror(MSG); \
    abort(); \
  }

/* Return a string describing the OpenCL error code 'e'.
 */
const char *cl_error_to_str(cl_int e);

/* Print a list of available OpenCL platforms and devices
 * to standard output.
 */
void print_platforms_devices();

/* Create an OpenCL context and a matching command queue on a platform from a
 * vendor whose name contains 'plat_name' on a device whose name contains
 * 'dev_name'. Both 'plat_name' and 'dev_name' may be NULL, indicating no
 * preference in the matter.
 *
 * If multiple devices match both 'plat_name' and 'dev_name', then 'idx'
 * prescribes the number of the device that should be chosen.
 *
 * You may also use the special value CHOOSE_INTERACTIVELY to offer the user
 * a choice. You should use this value for code you turn in.
 *
 * This function always succeeds. (If an error occurs, the program
 * is aborted.
 *
 * You can force interactive querying by defining the
 * CL_HELPER_FORCE_INTERACTIVE macro when compiling cl-helper.c.
 * You may do so by passing the -DCL_HELPER_FORCE_INTERACTIVE
 * compiler option.
 */
extern const char *CHOOSE_INTERACTIVELY;
void create_context_on(const char *plat_name, const char*dev_name, cl_uint
    idx, cl_context *ctx, cl_command_queue *queue, int enable_profiling);

/* Read contents of file 'filename'.
 * Return as a new string. You must free the string when you're done with it.
 *
 * This function always succeeds. (If an error occurs, the program
 * is aborted.
 */
char *read_file(const char *filename);

/* Create a new OpenCL kernel from the code in the string 'knl'.
 * 'knl_name' is the name of the kernel function, and 'options',
 * if not NULL, is a string containing compiler flags.
 *
 * You must release the resulting kernel when you're done
 * with it.
 *
 * This function always succeeds. (If an error occurs, the program
 * is aborted.
 */
cl_kernel kernel_from_string(cl_context ctx, 
    char const *knl, char const *knl_name, char const *options);

/* Print information about a device, found from either the
 * queue or the device_id.
 */
void print_device_info(cl_device_id device);
void print_device_info_from_queue(cl_command_queue queue);

#define SET_1_KERNEL_ARG(knl, arg0) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0));

#define SET_2_KERNEL_ARGS(knl, arg0, arg1) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1));

#define SET_3_KERNEL_ARGS(knl, arg0, arg1, arg2) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2));

#define SET_4_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3));

#define SET_5_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4));

#define SET_6_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5));

#define SET_7_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5, arg6) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 6, sizeof(arg6), &arg6));

#define SET_8_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 6, sizeof(arg6), &arg6)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 7, sizeof(arg7), &arg7));

#define SET_9_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 6, sizeof(arg6), &arg6)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 7, sizeof(arg7), &arg7)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 8, sizeof(arg8), &arg8));

#define SET_10_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 6, sizeof(arg6), &arg6)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 7, sizeof(arg7), &arg7)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 8, sizeof(arg8), &arg8)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 9, sizeof(arg9), &arg9));

#define SET_11_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 6, sizeof(arg6), &arg6)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 7, sizeof(arg7), &arg7)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 8, sizeof(arg8), &arg8)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 9, sizeof(arg9), &arg9)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 10, sizeof(arg10), &arg10));

#define SET_12_KERNEL_ARGS(knl, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 0, sizeof(arg0), &arg0)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 1, sizeof(arg1), &arg1)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 2, sizeof(arg2), &arg2)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 3, sizeof(arg3), &arg3)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 4, sizeof(arg4), &arg4)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 5, sizeof(arg5), &arg5)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 6, sizeof(arg6), &arg6)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 7, sizeof(arg7), &arg7)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 8, sizeof(arg8), &arg8)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 9, sizeof(arg9), &arg9)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 10, sizeof(arg10), &arg10)); \
  CALL_CL_GUARDED(clSetKernelArg, (knl, 11, sizeof(arg11), &arg11));

#endif
