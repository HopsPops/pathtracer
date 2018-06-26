#pragma once

#include <aabb.hpp>
#include <memory>

class Ray;
class Vector3;

std::unique_ptr<Vector3> intersectTriangle(const Ray&, const Vector3&, const Vector3&, const Vector3&);
std::unique_ptr<Vector3> intersectTriangle(const Ray&, const Triangle&);
bool intersectAABB(const Ray&, const Vector3&, const Vector3&);
bool intersectAABB(const Ray&, const AABB&);
