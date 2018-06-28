#include "math.hpp"
#pragma once
#include "triangle.hpp"

class AABB {
	private:
	public:
		Vector3 minimum, maximum;
		AABB(Vector3, Vector3);
		AABB(const Triangles&);
		AABB();
		Vector3 getMin() const;
		Vector3 getMax() const;
};
