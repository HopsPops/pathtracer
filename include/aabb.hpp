#pragma once
#include "types.hpp"

class AABB {
	private:
		Vec3 min, max;
	public:
		AABB(Vec3, Vec3);
		AABB();
		Vec3 getMin() const;
		Vec3 getMax() const;
};
