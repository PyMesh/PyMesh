/*
 * Copyright (c) 2012 Masaki Saito
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

#pragma once

#include <algorithm>
#include <complex>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

#if defined (__GLIBC__)
# include <endian.h>
# if (__BYTE_ORDER == __LITTLE_ENDIAN)
#  define AOBA_NUMPY_LITTLE_ENDIAN
# elif (__BYTE_ORDER == __BIG_ENDIAN)
#  define AOBA_NUMPY_BIG_ENDIAN
# elif (__BYTE_ORDER == __PDP_ENDIAN)
#  define AOBA_NUMPY_PDP_ENDIAN
# else
#  error Unknown machine endianness detected.
# endif
# define AOBA_NUMPY_BYTE_ORDER __BYTE_ORDER
#elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN) || \
    defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__) || \
    defined(_STLP_BIG_ENDIAN) && !defined(_STLP_LITTLE_ENDIAN)
# define AOBA_NUMPY_BIG_ENDIAN
# define AOBA_NUMPY_BYTE_ORDER 4321
#elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN) || \
    defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) || \
    defined(_STLP_LITTLE_ENDIAN) && !defined(_STLP_BIG_ENDIAN)
# define AOBA_NUMPY_LITTLE_ENDIAN
# define AOBA_NUMPY_BYTE_ORDER 1234
#elif defined(__sparc) || defined(__sparc__) \
   || defined(_POWER) || defined(__powerpc__) \
   || defined(__ppc__) || defined(__hpux) || defined(__hppa) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(__s390__)
# define AOBA_NUMPY_BIG_ENDIAN
# define AOBA_NUMPY_BYTE_ORDER 4321
#elif defined(__i386__) || defined(__alpha__) \
   || defined(__ia64) || defined(__ia64__) \
   || defined(_M_IX86) || defined(_M_IA64) \
   || defined(_M_ALPHA) || defined(__amd64) \
   || defined(__amd64__) || defined(_M_AMD64) \
   || defined(__x86_64) || defined(__x86_64__) \
   || defined(_M_X64) || defined(__bfin__)

# define AOBA_NUMPY_LITTLE_ENDIAN
# define AOBA_NUMPY_BYTE_ORDER 1234
#else
# error The file aoba/IO/Numpy.hpp needs to be set up for your CPU type.
#endif

namespace aoba {
namespace detail {

inline uint16_t ReorderInteger(const uint16_t& x) {
#ifdef AOBA_NUMPY_BIG_ENDIAN
    uint16_t y;
    const char* rx = reinterpret_cast<const char*>(&x);
    char* ry = reinterpret_cast<char*>(&y);
    *ry = *(rx + 1);
    *(ry + 1) = *rx;
    return y;
#else
    return x;
#endif
}

template<typename Scalar>
struct DescriptorDataType {};
template<>
struct DescriptorDataType<double> { static const char value = 'f'; };
template<>
struct DescriptorDataType<float> { static const char value = 'f'; };
template<>
struct DescriptorDataType<int> { static const char value = 'i'; };
template<>
struct DescriptorDataType<std::complex<float> > { static const char value = 'c'; };
template<>
struct DescriptorDataType<std::complex<double> > { static const char value = 'c'; };

template<typename Scalar>
inline std::string CreateDescriptor() {
    std::string descriptor;
#ifdef AOBA_NUMPY_LITTLE_ENDIAN
    descriptor.push_back('<');
#else
    descriptor.push_back('>');
#endif
    descriptor.push_back(DescriptorDataType<Scalar>::value);
    std::stringstream stream;
    stream << sizeof(Scalar);
    descriptor.append(stream.str());
    return descriptor;
}

inline void CreateMetaData(
    std::string& preamble, std::string& header,
    const std::string& descriptor, bool fortran_order,
    int n_dims, const int shape[])
{
    header = "{'descr': '";
    header.append(descriptor);
    if(fortran_order) {
        header.append("', 'fortran_order': True, ");
    } else {
        header.append("', 'fortran_order': False, ");
    }
    header.append("'shape': (");
    std::stringstream shape_stream;
    if(1 < n_dims) {
        for(int d=0; d<n_dims; ++d) {
            shape_stream << shape[d];
            if(d + 1 != n_dims) {
                shape_stream << ",";
            }
        }
    } else {
        shape_stream << shape[0] << ",";
    }
    shape_stream << "), }";
    header.append(shape_stream.str());
    const int to_padding = 16 - (10 + header.size() + 1) % 16;
    for(int m=0; m<to_padding; ++m) {
        header.push_back(' ');
    }
    header.push_back('\n');

    preamble = "\x93NUMPY";
    preamble.push_back((char)1);
    preamble.push_back((char)0);
    uint16_t header_length = detail::ReorderInteger((uint16_t)header.size());
    preamble.append(reinterpret_cast<char*>(&header_length), 2);
}

} // namespace detail

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename, bool fortran_order,
    int n_dims, const int shape[], const Scalar* data)
{
    if(n_dims <= 0)
        throw std::invalid_argument("received an invalid argument");

    std::string preamble, header;
    std::string descriptor = detail::CreateDescriptor<Scalar>();
    detail::CreateMetaData(
        preamble, header, descriptor, fortran_order, n_dims, shape);
    const size_t metadata_length = preamble.size() + header.size();
    if(metadata_length % 16 != 0) {
        throw std::runtime_error(
            "formatting error: metadata length is not divisible by 16.");
    }
    std::ofstream stream(
        filename.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
    if(!stream) {
        throw std::runtime_error("io error: failed to open a file.");
    }
    stream << preamble << header;
    int size = 1;
    for(int i=0; i<n_dims; ++i) { size *= shape[i]; }
    stream.write(reinterpret_cast<const char*>(data), sizeof(Scalar)*size);
}

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename,
    int n_dims, const int shape[], const Scalar* data)
{
    SaveArrayAsNumpy(
        filename, false, n_dims, shape, data);
}

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename, const std::vector<Scalar>& data) 
{
    const int length = (int)data.size();
    SaveArrayAsNumpy(filename, false, 1, &length, &data[0]);
}

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename, int x0, const Scalar* data)
{
    SaveArrayAsNumpy(filename, false, 1, &x0, data); 
}

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename, int x0, int x1, const Scalar* data)
{
    const int dim[2] = { x0, x1 };
    SaveArrayAsNumpy(filename, false, 2, dim, data); 
}

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename, int x0, int x1, int x2, const Scalar* data)
{
    const int dim[3] = { x0, x1, x2 };
    SaveArrayAsNumpy(filename, false, 3, dim, data); 
}

template<typename Scalar>
void SaveArrayAsNumpy(
    const std::string& filename, int x0, int x1, int x2, int x3, const Scalar* data)
{
    const int dim[4] = { x0, x1, x2, x3 };
    SaveArrayAsNumpy(filename, false, 4, dim, data); 
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename, std::vector<int>& shape,
    std::vector<Scalar>& data)
{
    std::ifstream stream(filename.c_str(), std::ios::in|std::ios::binary);
    if(!stream) {
        throw std::runtime_error("io error: failed to open a file.");
    }
    // check if this file is the valid .npy file
    std::string valid_preamble = "\x93NUMPY";
    valid_preamble.push_back(char(1));
    valid_preamble.push_back(char(0));
    std::string preamble(8, ' ');
    stream.read(&preamble[0], 8);
    if(valid_preamble != preamble) {
        throw std::runtime_error(
            "io error: this file do not have a valid npy format.");
    }
    // load header
    uint16_t header_length;
    stream.read(reinterpret_cast<char*>(&header_length), sizeof(uint16_t));
    header_length = detail::ReorderInteger(header_length);
    if((header_length + preamble.size() + sizeof(uint16_t)) % 16 != 0) {
        throw std::runtime_error(
            "formatting error: metadata length is not divisible by 16.");
    }
    std::string header(header_length, ' ');
    stream.read(reinterpret_cast<char*>(&header[0]), header_length);

    // load fortran order
    typedef std::string::size_type size_type;
    const size_type header_loc = header.find("fortran_order") + 16;
    const bool fortran_order = (header.substr(header_loc, 4) == "True");

    // load shape
    const size_type shape_loc1 = header.find("(");
    const size_type shape_loc2 = header.find(")");
    std::string shape_str = header.substr(
        shape_loc1 + 1, shape_loc2 - shape_loc1 - 1);
    if(shape_str[shape_str.size() - 1] == ',') shape.resize(1);
    else shape.resize(std::count(shape_str.begin(), shape_str.end(), ',') + 1);
    for(size_t i=0; i<shape.size(); ++i) {
        std::stringstream ss;
        const size_type loc = shape_str.find(",");
        ss << shape_str.substr(0, loc);
        ss >> shape[i];
        shape_str = shape_str.substr(loc + 1);
    }
    if(fortran_order) {
        std::reverse(shape.begin(), shape.end());
    }

    // load descriptor
    const size_type descr_loc = header.find("descr") + 9;
    const char endian_str = header[descr_loc];
    const bool little_endian = (endian_str == '<' || endian_str == '|');
#ifdef AOBA_NUMPY_BIG_ENDIAN
    const bool is_same_endian = !little_endian;
#else
    const bool is_same_endian = little_endian;
#endif
    if(!is_same_endian) {
        throw std::runtime_error(
            "formatting error: difference endian is not supported.");
    }
    const char data_type = header[descr_loc + 1];
    size_t word_size;
    std::stringstream ss;
    ss << header[descr_loc + 2];
    ss >> word_size;
    if(data_type != detail::DescriptorDataType<Scalar>::value ||
       word_size != sizeof(Scalar)) {
        throw std::runtime_error(
            "formatting error: the type of .npy file is not equal to that of std::vector<T>");
    }

    // load data
    size_t total = 1;
    for(size_t i=0; i<shape.size(); ++i) {
        total *= shape[i];
    }
    data.resize(total);
    stream.read(reinterpret_cast<char*>(&data[0]), word_size*total);
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename, std::vector<Scalar>& data)
{
    std::vector<int> tmp_dim;
    LoadArrayFromNumpy(filename, tmp_dim, data);
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename, int shape[], std::vector<Scalar>& data)
{
    std::vector<int> tmp_dim;
    LoadArrayFromNumpy(filename, tmp_dim, data);
    for(size_t i=0; i<tmp_dim.size(); ++i) shape[i] = tmp_dim[i];
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename,
    int& x0, std::vector<Scalar>& data)
{
    std::vector<int> tmp_dim;
    LoadArrayFromNumpy(filename, tmp_dim, data);
    if(tmp_dim.size() != 1) {
        throw std::runtime_error("io error: the dimension of array is different from the expected one.");
    }
    x0 = tmp_dim[0];
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename,
    int& x0, int& x1, std::vector<Scalar>& data)
{
    std::vector<int> tmp_dim;
    LoadArrayFromNumpy(filename, tmp_dim, data);
    if(tmp_dim.size() != 2) {
        throw std::runtime_error("io error: the dimension of array is different from the expected one.");
    }
    x0 = tmp_dim[0];
    x1 = tmp_dim[1];
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename,
    int& x0, int& x1, int& x2, std::vector<Scalar>& data)
{
    std::vector<int> tmp_dim;
    LoadArrayFromNumpy(filename, tmp_dim, data);
    if(tmp_dim.size() != 3) {
        throw std::runtime_error("io error: the dimension of array is different from the expected one.");
    }
    x0 = tmp_dim[0];
    x1 = tmp_dim[1];
    x2 = tmp_dim[2];
}

template<typename Scalar>
void LoadArrayFromNumpy(
    const std::string& filename,
    int& x0, int& x1, int& x2, int& x3, std::vector<Scalar>& data)
{
    std::vector<int> tmp_dim;
    LoadArrayFromNumpy(filename, tmp_dim, data);
    if(tmp_dim.size() != 4) {
        throw std::runtime_error("io error: the dimension of array is different from the expected one.");
    }
    x0 = tmp_dim[0];
    x1 = tmp_dim[1];
    x2 = tmp_dim[2];
    x3 = tmp_dim[3];
}

} // namespace aoba
