#pragma once

#include "types.hpp"
#include "ray.hpp"

bool intersectTriangle(ray&, vec3, vec3, vec3, vec3&);
bool intersectAABB(ray&, vec3, vec3);
