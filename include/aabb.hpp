#pragma once

#include <math.hpp>
#include <triangle.hpp>
#include <vector>

class AABB {
	private:
	public:
		Vector3 minimum, maximum;
		AABB(const Vector3&, const Vector3&);
		AABB(const Triangles&);
		AABB(const std::vector<Vector3>&);
		AABB(const AABB& aabb1, const AABB& aabb2);
		AABB();
		const Vector3& getMin() const;
		const Vector3& getMax() const;
		Vector3 center() const;
};
