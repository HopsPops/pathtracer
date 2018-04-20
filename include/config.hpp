#pragma once
#include <iostream>
#include "types.hpp"

class Config {
	public:
		char input[500] = { 0 };
		char output[500] = { 0 };
		unsigned int k = 0;
		unsigned int xres = 800;
		unsigned int yres = 600;
		vec3 viewPoint = vec3(0.0f, 0.0f, 0.0f);
		vec3 lookAt = vec3(0.0f, 0.0f, 1.0f);
		vec3 up = vec3(0.0f, 1.0f, 0.0f);
		float yview;

		void print(std::ostream& stream) {
			stream << "input: " << input << std::endl;
			stream << "output: " << output << std::endl;
			stream << "k: " << k << std::endl;
			stream << "xres: " << xres << std::endl;
			stream << "yres: " << xres << std::endl;
			stream << "VPx: " << viewPoint.x << std::endl;
			stream << "VPy: " << viewPoint.y << std::endl;
			stream << "VPz: " << viewPoint.z << std::endl;
			stream << "LAx: " << lookAt.x << std::endl;
			stream << "LAy: " << lookAt.y << std::endl;
			stream << "LAz: " << lookAt.z << std::endl;
			stream << "UPx: " << up.x << std::endl;
			stream << "UPy: " << up.y << std::endl;
			stream << "UPz: " << up.z << std::endl;
			stream << "yview: " << yview << std::endl;
		}

};
