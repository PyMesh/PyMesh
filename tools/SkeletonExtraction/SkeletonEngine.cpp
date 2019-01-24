#include <sstream>
#include <string>
#include <Core/Exception.h>

#include "SkeletonEngine.h"

#ifdef WITH_CGAL
#include "CGAL/CreateSkeleton2.h"
#endif

using namespace PyMesh;

SkeletonEngine::Ptr SkeletonEngine::create(size_t dim, const std::string& library_name)
{
    if (library_name == "auto") {
#if WITH_CGAL
    return SkeletonEngine::create(dim, "cgal");
#endif
    }

#ifdef WITH_CGAL
    if (library_name == "cgal") {
        if (dim == 2) {
            return std::make_shared<CreateSkeleton2>();
        } 
        else {
            std::stringstream err_msg;
            err_msg << "CGAL Skeleton extraction does not support dim=" << dim;
            throw NotImplementedError(err_msg.str());
        }
    }
#endif
}

bool SkeletonEngine::supports(const std::string& library_name)
{
#ifdef WITH_CGAL
    if ((library_name) == "cgal") return true;
#endif
}

std::vector<std::string> SkeletonEngine::get_available_engines()
{
    std::vector<std::string> engine_names;
#ifdef WITH_CGAL
    engine_names.push_back("cgal");
#endif
    return engine_names;
}
