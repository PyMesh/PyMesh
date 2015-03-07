#pragma once

#include <SelfIntersectionResolver.h>

class IGLSelfIntersectionResolver : public SelfIntersectionResolver {
    public:
        virtual ~IGLSelfIntersectionResolver() {}

    public:
        virtual void run();
};
