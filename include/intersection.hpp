#pragma once

#include "ray.hpp"
#include "aabb.hpp"
#include "math.hpp"

bool intersectTriangle(const Ray&, Vector3, Vector3, Vector3, Vector3&);
bool intersectAABB(const Ray&, Vector3, Vector3);
bool intersectAABB(const Ray&, const AABB&);
