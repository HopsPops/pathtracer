#pragma once

#include "math.hpp"
#include <camera.hpp>
#include <iostream>

class Ray {
	private:
		Vector3 origin, direction;

	public:
		Ray(Vector3 origin, Vector3 direction);

		static Ray createRay(const Camera&, int, int, int, int, double);

		const Vector3& getOrigin() const;

		const Vector3& getDirection() const;

		void print(std::ostream& stream);
};

