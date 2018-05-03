#pragma once
#include "types.hpp"

class Transformation {
	private:
		float scale = 1.0;
		Vec3 translation { 0.0f, 0.0f, 0.0f };

	public:
		Transformation() {};
		void setScale(float);
		void setTranslation(Vec3);
		Mat4 matrix();
};

