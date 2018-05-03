#include <ray.hpp>
#include "types.hpp"

Ray::Ray(Vec3 origin, Vec3 direction) :
		origin(origin), direction(direction) {
}
;

Ray Ray::createRay(Camera& c, int screenX, int screenY, int width, int height, double aspectRatio) {
	float x = (2.0f * screenX) / width - 1.0f;
	float y = 1.0f - (2.0f * screenY) / height;

	Vec4 rayClip(x, y, -1.0f, 1.0f);

	Mat4 projectionInverse = glm::inverse(c.perspective(aspectRatio));

	Vec4 rayEye = projectionInverse * rayClip;
	rayEye.z = -1.0f;
	rayEye.w = 0.0f;

	Mat4 viewInverse = glm::inverse(c.view());

	Vec4 rayWorld = viewInverse * rayEye;
	Vec3 rayDirection = glm::normalize(Vec3(rayWorld));

	return Ray(c.getPosition(), rayDirection);
}

Vec3 Ray::getOrigin() const {
	return origin;
}

Vec3 Ray::getDirection() const {
	return direction;
}

void Ray::print(std::ostream& stream) {
	stream << "origin: {" << origin.x << ", " << origin.y << ", " << origin.z << "}";
	stream << " ";
	stream << "dir: {" << direction.x << ", " << direction.y << ", " << direction.z << "}";
	stream << "\n";
}
