#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <Core/EigenTypedef.h>
#include <CGAL/SelfIntersection.h>

#include <CGAL/enum.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

namespace py = pybind11;
using namespace PyMesh;

void init_CGAL(py::module &m) {
    py::class_<SelfIntersection, std::shared_ptr<SelfIntersection> >(m, "SelfIntersection")
        .def(py::init<const MatrixFr& , const MatrixIr&>())
        .def("detect_self_intersection",
                &SelfIntersection::detect_self_intersection)
        .def("clear", &SelfIntersection::clear)
        .def("get_self_intersecting_pairs",
                &SelfIntersection::get_self_intersecting_pairs)
        .def("handle_intersection_candidate",
                &SelfIntersection::handle_intersection_candidate);

    py::class_<CGAL::Gmpz>(m, "Gmpz")
        .def(py::init<>())
        .def(py::init<int>())
        .def(py::init<Float>())
        .def_property_readonly("sign", [](const CGAL::Gmpz& v){
                    switch (v.sign()) {
                    case CGAL::ZERO:
                        return 0;
                    case CGAL::POSITIVE:
                        return 1;
                    case CGAL::NEGATIVE:
                        return -1;
                    default:
                        throw RuntimeError("Unexpected sign type");
                    }
                })
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self + int())
        .def(int() + py::self)
        .def(py::self - int())
        .def(int() - py::self)
        .def(py::self * int())
        .def(int() * py::self)
        .def(py::self / int())
        .def(int() / py::self)
        .def(py::self += py::self)
        .def(py::self += int())
        .def(py::self -= py::self)
        .def(py::self -= int())
        .def(py::self *= py::self)
        .def(py::self *= int())
        .def(py::self /= py::self)
        .def(py::self /= int())
        .def(py::self << int())
        .def(py::self >> int())
        .def(py::self <<= int())
        .def(py::self >>= int())
        .def(py::self & py::self)
        .def(py::self &= py::self)
        .def(py::self | py::self)
        .def(py::self |= py::self)
        .def(py::self ^ py::self)
        .def(py::self ^= py::self)
        .def_property_readonly("size", &CGAL::Gmpz::size)
        .def_property_readonly("bit_size", &CGAL::Gmpz::bit_size)
        .def_property_readonly("approximate_decimal_length", &CGAL::Gmpz::approximate_decimal_length)
        .def("to_double", &CGAL::Gmpz::to_double)
        .def("__repr__", [](const CGAL::Gmpz& v) {
                std::stringstream sout;
                sout << v;
                return sout.str();
                }) ;

    py::class_<CGAL::Gmpq>(m, "Gmpq")
        .def(py::init<>())
        .def(py::init<int>())
        .def(py::init<CGAL::Gmpz>())
        .def(py::init<int, int>())
        .def(py::init<CGAL::Gmpz, CGAL::Gmpz>())
        .def(py::init<Float>())
        .def(py::init<std::string>())
        .def(py::init<std::string, int>())
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)

        .def(py::self + int())
        .def(py::self + CGAL::Gmpz())
        .def(py::self + Float())
        .def(int()        + py::self)
        .def(CGAL::Gmpz() + py::self)
        .def(Float()      + py::self)

        .def(py::self - int())
        .def(py::self - CGAL::Gmpz())
        .def(py::self - Float())
        .def(int()        - py::self)
        .def(CGAL::Gmpz() - py::self)
        .def(Float()      - py::self)

        .def(py::self * int())
        .def(py::self * CGAL::Gmpz())
        .def(py::self * Float())
        .def(int() * py::self)
        .def(CGAL::Gmpz() * py::self)
        .def(Float() * py::self)

        .def(py::self / int())
        .def(py::self / CGAL::Gmpz())
        .def(py::self / Float())
        .def(int() / py::self)
        .def(CGAL::Gmpz() / py::self)
        .def(Float() / py::self)

        .def(py::self += py::self)
        .def(py::self += CGAL::Gmpz())
        .def(py::self += int())
        .def(py::self += Float())

        .def(py::self -= py::self)
        .def(py::self -= CGAL::Gmpz())
        .def(py::self -= int())
        .def(py::self -= Float())

        .def(py::self *= py::self)
        .def(py::self *= CGAL::Gmpz())
        .def(py::self *= int())
        .def(py::self *= Float())

        .def(py::self /= py::self)
        .def(py::self /= CGAL::Gmpz())
        .def(py::self /= int())
        .def(py::self /= Float())

        .def_property_readonly("numerator", &CGAL::Gmpq::numerator)
        .def_property_readonly("denominator", &CGAL::Gmpq::denominator)
        .def("to_double", &CGAL::Gmpq::to_double)
        .def("__repr__", [](const CGAL::Gmpq& v) {
                std::stringstream sout;
                sout << v;
                return sout.str();
                }) ;

}
