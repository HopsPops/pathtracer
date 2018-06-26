#pragma once

#include <aabb.hpp>

class Ray;
class Vector3;

bool intersectTriangle(const Ray&, Vector3, Vector3, Vector3, Vector3&);
bool intersectTriangle(const Ray&, const Triangle&, Vector3&);
bool intersectAABB(const Ray&, Vector3, Vector3);
bool intersectAABB(const Ray&, const AABB&);
