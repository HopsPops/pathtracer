#pragma once

#include "ray.hpp"
#include "aabb.hpp"
#include "types.hpp"

bool intersectTriangle(const Ray&, Vec3, Vec3, Vec3, Vec3&);
bool intersectAABB(const Ray&, Vec3, Vec3);
bool intersectAABB(const Ray&, const AABB&);
