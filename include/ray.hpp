#pragma once

#include <camera.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "types.hpp"
#include <iostream>

class Ray {
	private:
		Vec3 origin, direction;

	public:
		Ray(Vec3 origin, Vec3 direction);

		static Ray createRay(Camera&, int, int, int, int, double);

		Vec3 getOrigin() const;

		Vec3 getDirection() const;

		void print(std::ostream& stream);
};

