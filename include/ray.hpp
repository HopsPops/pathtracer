#pragma once

#include "math.hpp"
#include <camera.hpp>
#include <iostream>

class Ray {
	private:
		Vector3 origin, direction;

	public:
		Ray(Vector3 origin, Vector3 direction);

		static Ray createRay(Camera&, int, int, int, int, double);

		Vector3 getOrigin() const;

		Vector3 getDirection() const;

		void print(std::ostream& stream);
};

