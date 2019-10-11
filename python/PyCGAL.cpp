#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <Core/EigenTypedef.h>

#if WITH_CGAL
#include <CGAL/SelfIntersection.h>
#include <CGAL/enum.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Arrangement2.h>
#include <CGAL/SnapRounding2.h>
#include <CGAL/StraightSkeleton.h>
#endif

namespace py = pybind11;

void init_CGAL(py::module &m) {
#if WITH_CGAL
    using namespace PyMesh;
    py::class_<SelfIntersection, std::shared_ptr<SelfIntersection> >(m, "SelfIntersection")
        .def(py::init<const MatrixFr& , const MatrixIr&>())
        .def("detect_self_intersection",
                &SelfIntersection::detect_self_intersection)
        .def("clear", &SelfIntersection::clear)
        .def("get_self_intersecting_pairs",
                &SelfIntersection::get_self_intersecting_pairs)
        .def("handle_intersection_candidate",
                &SelfIntersection::handle_intersection_candidate);

    py::class_<Arrangement2, std::shared_ptr<Arrangement2> > arrangement(m, "Arrangement2");
    arrangement.def(py::init<>())
        .def_property("points",
                &Arrangement2::get_points,
                &Arrangement2::set_points,
                py::return_value_policy::reference_internal)
        .def_property("segments",
                &Arrangement2::get_segments,
                &Arrangement2::set_segments,
                py::return_value_policy::reference_internal)
        .def("run", &Arrangement2::run)
        .def_property_readonly("vertices",
                &Arrangement2::get_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("edges",
                &Arrangement2::get_edges,
                py::return_value_policy::reference_internal)
        .def("query", &Arrangement2::query);

    py::enum_<Arrangement2::ElementType>(arrangement, "ElementType")
        .value("POINT", Arrangement2::POINT)
        .value("SEGMENT", Arrangement2::SEGMENT)
        .value("CELL", Arrangement2::CELL)
        .export_values();

    py::class_<SnapRounding2, std::shared_ptr<SnapRounding2> >(m, "SnapRounding2")
        .def(py::init<>())
        .def_property("points",
                &SnapRounding2::get_points,
                &SnapRounding2::set_points,
                py::return_value_policy::reference_internal)
        .def_property("segments",
                &SnapRounding2::get_segments,
                &SnapRounding2::set_segments,
                py::return_value_policy::reference_internal)
        .def("run", &SnapRounding2::run)
        .def_property_readonly("vertices",
                &SnapRounding2::get_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("edges",
                &SnapRounding2::get_edges,
                py::return_value_policy::reference_internal);

    py::class_<StraightSkeleton, std::shared_ptr<StraightSkeleton> >(m, "StraightSkeleton")
        .def(py::init<>())
        .def("run", &StraightSkeleton::run)
        .def_property_readonly("points",
                &StraightSkeleton::get_skeleton_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("segments",
                &StraightSkeleton::get_skeleton_edges,
                py::return_value_policy::reference_internal);


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
#endif
}
