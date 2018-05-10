#pragma once
#include "math.hpp"

class AABB {
	private:
		Vector3 min, max;
	public:
		AABB(Vector3, Vector3);
		AABB();
		Vector3 getMin() const;
		Vector3 getMax() const;
};
