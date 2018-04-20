#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "camera.hpp"
#include "types.hpp"
#include <iostream>

class ray {
	private:
		vec3 origin, direction;

	public:
		ray(vec3 origin, vec3 direction);

		static ray createRay(camera&, int, int, int, int, double);

		vec3 getOrigin();

		vec3 getDirection();

		void print(std::ostream& stream);
};

