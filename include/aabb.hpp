#include "math.hpp"
#pragma once
#include "triangle.hpp"

class AABB {
	private:
	public:
		Vector3 minimum, maximum;
		AABB(const Vector3&, const Vector3&);
		AABB(const Triangles&);
		AABB();
		const Vector3& getMin() const;
		const Vector3& getMax() const;
};
