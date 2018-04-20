#include "ray.hpp"
#include "types.hpp"

ray::ray(vec3 origin, vec3 direction) :
		origin(origin), direction(direction) {
}
;

ray ray::createRay(camera& c, int screenX, int screenY, int width, int height, double aspectRatio) {
	float x = (2.0f * screenX) / width - 1.0f;
	float y = 1.0f - (2.0f * screenY) / height;

	vec4 rayClip(x, y, -1.0f, 1.0f);

	mat4 projectionInverse = glm::inverse(c.perspective(aspectRatio));

	vec4 rayEye = projectionInverse * rayClip;
	rayEye.z = -1.0f;
	rayEye.w = 0.0f;

	mat4 viewInverse = glm::inverse(c.view());

	vec4 rayWorld = viewInverse * rayEye;
	vec3 rayDirection = glm::normalize(vec3(rayWorld));

	return ray(c.getPosition(), rayDirection);
}

vec3 ray::getOrigin() {
	return origin;
}

vec3 ray::getDirection() {
	return direction;
}

void ray::print(std::ostream& stream) {
	stream << "origin: {" << origin.x << ", " << origin.y << ", " << origin.z << "}";
	stream << " ";
	stream << "dir: {" << direction.x << ", " << direction.y << ", " << direction.z << "}";
	stream << "\n";
}
