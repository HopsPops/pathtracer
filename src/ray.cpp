#include <ray.hpp>

Ray::Ray(Vector3 origin, Vector3 direction) :
		origin(origin), direction(direction.normalize()) {
};

Ray Ray::createRay(const Camera& c, int screenX, int screenY, int width, int height, double aspectRatio) {
	float x = (2.0f * screenX) / width - 1.0f;
	float y = 1.0f - (2.0f * screenY) / height;

	Vector4 rayClip(x, y, -1.0f, 1.0f);

	Matrix4x4 projectionInverse = c.perspective(aspectRatio).inverse();

	Vector4 rayEye = projectionInverse * rayClip;
	rayEye.z = -1.0f;
	rayEye.w = 0.0f;

	Matrix4x4 viewInverse = c.view().inverse();

	Vector4 rayWorld = viewInverse * rayEye;

	return Ray(c.getPosition(), ((Vector3) rayWorld).normalize());
}

const Vector3& Ray::getOrigin() const {
	return origin;
}

const Vector3& Ray::getDirection() const {
	return direction;
}

void Ray::print(std::ostream& stream) {
	stream << "origin: {" << origin.x << ", " << origin.y << ", " << origin.z << "}";
	stream << " ";
	stream << "dir: {" << direction.x << ", " << direction.y << ", " << direction.z << "}";
	stream << "\n";
}
