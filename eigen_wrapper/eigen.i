/**
Author: Qingnan Zhou (I don't normally label my code, but this piece is so
experimental.  So labeling it will enable me to quickly get back to it later on.)

Bits (or large chunks of code) from
http://code.google.com/p/b-tk/source/browse/BTK/branches/experimental/Utilities/SWIG/eigen.i

Warning: This interface is designed for projects using C++ as the main language
for data storage and number crunching and using python for fast prototyping.
In particular, C++ code is responsible to allocate/release
memories, and once allocated, memory pointer in C++ are valid for a relatively
long period.  On the other hand, python is used "play" with the data generated
by C++, its ndarray object could be created and goes out of scope in quick
successions.  Based on this spirit, whenever data is passed from C++ to python,
it is seamlessly wrapped if possible, and whenever data is passed from python to
C++, it is copied.

Note:
Eigen store vector as either row matrix or column matrix, so when converting
Eigen data structure to ndarray, the result will always be 2 dimensional.

Know problems:
* Only works with numpy version < 1.7.

**/

/**
   numpy.i should have been included and initialized before
**/

%fragment("eigen_type_map", "header",
        fragment="NumPy_Fragments") {

    template <typename T> struct NumpyType {
        static int getCode() {return -1;}
    };

    template<> struct NumpyType<double> {
        static int getCode() {return PyArray_DOUBLE;}
    };

    template<> struct NumpyType<float> {
        static int getCode() {return PyArray_FLOAT;}
    };

    template<> struct NumpyType<int> {
        static int getCode() {return PyArray_INT;}
    };

    template<> struct NumpyType<long> {
        static int getCode() {return PyArray_LONG;}
    };

    template<> struct NumpyType<short> {
        static int getCode() {return PyArray_SHORT;}
    };

    template<> struct NumpyType<char> {
        static int getCode() {return PyArray_CHAR;}
    };

    template<> struct NumpyType<unsigned char> {
        static int getCode() {return PyArray_UBYTE;}
    };

    template<> struct NumpyType<signed char> {
        static int getCode() {return PyArray_BYTE;}
    };

    /**
     * Create a python ndarray from the raw data.
     * @param[in] in       : The eigen based class for matrix
     * @param[in] data     : raw C array of bytes wrapped by eigen
     * @param[in] copy_data: if the raw data should be copied over to python
     * @returns : the PyObject respondes to ndarray with the data either
     *            correctly wrapped or copied.
     */
    template <class Derived>
    PyObject* ConvertFromEigenToNumpyMatrix(Eigen::MatrixBase<Derived>* in,
            void* data, bool copy_data=false) {
        npy_intp dims[2] = {in->rows(), in->cols()};
        bool row_major = in->Flags & Eigen::RowMajorBit;
        PyObject* array;
        if (!copy_data) {
            if (!row_major) {
                dims[0] = in->cols();
                dims[1] = in->rows();
            }
            array = PyArray_SimpleNewFromData(2, dims,
                        NumpyType<typename Derived::Scalar>::getCode(),
                        data);
            if (!array) return NULL;
            if (!row_major) {
                array = PyArray_Transpose((PyArrayObject*) array, NULL);
            }
        } else {
            array = PyArray_SimpleNew(2, dims,
                        NumpyType<typename Derived::Scalar>::getCode());
            if (!array) return NULL;
            // Copy data over.
            typename Derived::Scalar* py_data = static_cast<typename Derived::Scalar*>(PyArray_DATA(array));
            typename Derived::Scalar* cpp_data= static_cast<typename Derived::Scalar*>(data);
            for (int i = 0; i != dims[0]; ++i)
                for (int j = 0; j != dims[1]; ++j)
                    py_data[i*dims[1]+j] = in->coeff(i,j);
        }

        return array;
    }

    /**
     * Create an Eigen Matrix object from the data in python ndarray.
     * This method is copied from
     * http://code.google.com/p/b-tk/source/browse/BTK/branches/experimental/Utilities/SWIG/eigen.i?spec=svn973&r=973
     *
     * Current data have to be copied from numpy to eigen because eigen matrix
     * cannot be construct directly from C array.  Eigen provides a way to wrap
     * C array with Map, but when asigning a Map to Matrix data is copied
     * nontheless.
     * 
     * Furthermore, casting is carried out when numpy and Eigen uses different
     * types of scalar.  In particular, numpy.int is 64 bits and Eigen int is 32
     * bits, casting is required in such cases.
     */
    template <class Derived>
        void ConvertFromNumpyToEigenMatrix(Eigen::MatrixBase<Derived>* out,
                PyObject* in)
    {
        int rows = 0;
        int cols = 0;
        // Check object type
        if (!is_array(in))
        {
            PyErr_SetString(PyExc_ValueError, "The given input is not known as a NumPy array or matrix.");
            return;
        }
        // Check dimensions
        else if (array_numdims(in) > 2)
        {
            PyErr_SetString(PyExc_ValueError, "Eigen only support 1D or 2D array.");
            return;
        }
        else if (array_numdims(in) == 1)
        {
            rows = py_array_size(in,0);
            cols = 1;
            if ((Derived::RowsAtCompileTime != Eigen::Dynamic) && (Derived::RowsAtCompileTime != rows))
            {
                PyErr_SetString(PyExc_ValueError, "Row dimension mismatch between NumPy and Eigen objects (1D).");
                return;
            }
            else if ((Derived::ColsAtCompileTime != Eigen::Dynamic) && (Derived::ColsAtCompileTime != 1))
            {
                PyErr_SetString(PyExc_ValueError, "Column dimension mismatch between NumPy and Eigen objects (1D).");
                return;
            }
        }
        else if (array_numdims(in) == 2)
        {
            rows = py_array_size(in,0);
            cols = py_array_size(in,1);
            if ((Derived::RowsAtCompileTime != Eigen::Dynamic) && (Derived::RowsAtCompileTime != py_array_size(in,0)))
            {
                PyErr_SetString(PyExc_ValueError, "Row dimension mismatch between NumPy and Eigen objects (2D).");
                return;
            }
            else if ((Derived::ColsAtCompileTime != Eigen::Dynamic) && (Derived::ColsAtCompileTime != py_array_size(in,1)))
            {
                PyErr_SetString(PyExc_ValueError, "Column dimension mismatch between NumPy and Eigen objects (2D).");
                return;
            }
        }
        
        bool eigen_is_row_major = out->Flags & Eigen::RowMajorBit;
        bool numpy_is_row_major = array_is_contiguous(in);
        int eigen_type_code = NumpyType<typename Derived::Scalar>::getCode();
        int numpy_type_code = array_type(in);

        int eigen_order = NPY_ARRAY_BEHAVED | NPY_ARRAY_FORCECAST;
        if (eigen_is_row_major) {
            eigen_order |= NPY_ARRAY_C_CONTIGUOUS;
        } else {
            eigen_order |= NPY_ARRAY_F_CONTIGUOUS;
        }

        PyArray_Descr* dtype = PyArray_DescrFromType(eigen_type_code);
        PyArrayObject* arr = (PyArrayObject*)PyArray_FromAny(
                in, dtype, 0, 0, eigen_order, NULL);

        if (arr == NULL) {
            // Error already set by PyArray_FromAny
            return;
        }

        typename Derived::Scalar* data = static_cast<typename Derived::Scalar*>(PyArray_DATA(arr));
        Eigen::Map<Derived> array_wrapper(data, rows, cols);
        *out = array_wrapper;
    }
}

%define %EigenTypeMap(EigenType)
/**
 * Convert function return type EigenType* and EigenType& to numpy array types
 * No data is copied
 */
%typemap(out, fragment="NumPy_Fragments", fragment="eigen_type_map")
        EigenType*, EigenType& {
    PyObject* array = ConvertFromEigenToNumpyMatrix($1, $1->data());
    if (!array) SWIG_fail;
    $result = array;
}

/**
 * Convert function return type const EigenType* and const EigenType& to numpy array types
 * No data is copied
 */
%typemap(out, fragment="NumPy_Fragments", fragment="eigen_type_map")
        const EigenType*, const EigenType& {
    PyObject* array = ConvertFromEigenToNumpyMatrix($1, $1->data());
    if (!array) SWIG_fail;

    ((PyArrayObject* )array)->flags &= (~NPY_WRITEABLE);
    /**
     * The following code should be valid once I port numpy 1.7 into my machine.
     */
    //((PyArrayObject* )array)->flags &= (~NPY_ARRAY_WRITEABLE);
    $result = array;
}

/**
 * Convert function return type EigenType to numpy array types
 * Since it is return by value, all data will be copied over.
 */
%typemap(out, fragment="NumPy_Fragments",
        fragment="eigen_type_map") EigenType {
    PyObject* array = ConvertFromEigenToNumpyMatrix(&$1, $1.data(), true);
    if (PyErr_Occurred() != NULL) return NULL;
    if (!array) SWIG_fail;
    $result = array;
}

/**
 * Convert argument output of type EigenType to numpy array types.
 * The output will be appended in the return values.
 */
%typemap(in, numinputs=0, fragment="NumPy_Fragments",
        fragment="eigen_type_map") EigenType& ARGOUT(EigenType temp) {
    $1 = &temp;
}
%typemap(argout, fragment="NumPy_Fragments",
        fragments="eigen_type_map") EigenType& ARGOUT {
    PyObject* array = ConvertFromEigenToNumpyMatrix(&temp$argnum, temp$argnum.data(), true);
    if (PyErr_Occurred() != NULL) array=NULL;
    if (!array) SWIG_fail;
    $result = SWIG_Python_AppendOutput($result,array);
}

/**
 * Convert function argument from numpy array to EigenType* or const EigenType*
 * or EigenType& or const EigenType&.
 * Data will always be copied over.
 */
%typemap(in, fragment="NumPy_Fragments", fragment="eigen_type_map"
        //warning="999: Danger! C++ code wants pointer or reference of python memory.  I am going to do a copy of memory!"
        )
        EigenType* (EigenType temp),
        const EigenType* (EigenType temp),
        EigenType& (EigenType temp),
        const EigenType& (EigenType temp) {
    ConvertFromNumpyToEigenMatrix(&temp, $input);
    if (PyErr_Occurred() != NULL) return NULL;
    $1 = &temp;
}

/**
 * Convert function argument from numpy array to EigenType.
 * Data will always be copied over.
 */
%typemap(in, fragment="NumPy_Fragments", fragment="eigen_type_map")
        EigenType {
    ConvertFromNumpyToEigenMatrix(&$1, $input);
    if (PyErr_Occurred() != NULL) return NULL;
}
%enddef

/**
  This is not an exhaustive map of all eigen types.
  Because Eigen is a heavily templated library, each template instantiation
  creates a new type, which has to be enumerated here.
**/
%EigenTypeMap(Eigen::Vector3d)
%EigenTypeMap(Eigen::Vector4d)
%EigenTypeMap(Eigen::VectorXd)
%EigenTypeMap(Eigen::Matrix3d)
%EigenTypeMap(Eigen::Matrix4d)
%EigenTypeMap(Eigen::MatrixXd)

%EigenTypeMap(Eigen::Vector3f)
%EigenTypeMap(Eigen::Vector4f)
%EigenTypeMap(Eigen::VectorXf)
%EigenTypeMap(Eigen::Matrix3f)
%EigenTypeMap(Eigen::Matrix4f)
%EigenTypeMap(Eigen::MatrixXf)

%EigenTypeMap(Eigen::Vector3i)
%EigenTypeMap(Eigen::Vector4i)
%EigenTypeMap(Eigen::VectorXi)
%EigenTypeMap(Eigen::Matrix3i)
%EigenTypeMap(Eigen::Matrix4i)
%EigenTypeMap(Eigen::MatrixXi)

/**
  The following typemap statements map types defined in EigenTypedef.h.
**/
%EigenTypeMap(PyMesh::Vector2F)
%EigenTypeMap(PyMesh::Vector3F)
%EigenTypeMap(PyMesh::Vector4F)
%EigenTypeMap(PyMesh::VectorF)
%EigenTypeMap(PyMesh::Matrix3F)
%EigenTypeMap(PyMesh::Matrix4F)
%EigenTypeMap(PyMesh::MatrixF)
%EigenTypeMap(PyMesh::MatrixFr)

%EigenTypeMap(PyMesh::Vector2I)
%EigenTypeMap(PyMesh::Vector3I)
%EigenTypeMap(PyMesh::Vector4I)
%EigenTypeMap(PyMesh::VectorI)
%EigenTypeMap(PyMesh::Matrix3I)
%EigenTypeMap(PyMesh::Matrix4I)
%EigenTypeMap(PyMesh::MatrixI)
%EigenTypeMap(PyMesh::MatrixIr)

