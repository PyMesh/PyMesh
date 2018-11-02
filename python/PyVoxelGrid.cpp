#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Envelope/VoxelGrid.h>

namespace py = pybind11;
using namespace PyMesh;

void init_VoxelGrid(py::module &m) {
    py::class_<VoxelGrid<2>, std::shared_ptr<VoxelGrid<2> > >(m, "VoxelGrid2D")
        .def(py::init<Float>())
        .def("insert_mesh", &VoxelGrid<2>::insert_mesh)
        .def("create_grid", &VoxelGrid<2>::create_grid)
        .def("erode", &VoxelGrid<2>::erode)
        .def("dilate", &VoxelGrid<2>::dilate)
        .def("remove_cavities", &VoxelGrid<2>::remove_cavities)
        .def("get_voxel_mesh", &VoxelGrid<2>::get_voxel_mesh);

    py::class_<VoxelGrid<3>, std::shared_ptr<VoxelGrid<3> > >(m, "VoxelGrid3D")
        .def(py::init<Float>())
        .def("insert_mesh", &VoxelGrid<3>::insert_mesh)
        .def("create_grid", &VoxelGrid<3>::create_grid)
        .def("erode", &VoxelGrid<3>::erode)
        .def("dilate", &VoxelGrid<3>::dilate)
        .def("remove_cavities", &VoxelGrid<3>::remove_cavities)
        .def("get_voxel_mesh", &VoxelGrid<3>::get_voxel_mesh);
}
