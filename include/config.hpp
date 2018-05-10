#pragma once
#include <iostream>
#include "math.hpp"

class Config {
	public:
		char input[500] = { 0 };
		char output[500] = { 0 };
		unsigned int k = 0;
		unsigned int xres = 800;
		unsigned int yres = 600;
		Vector3 viewPoint = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 lookAt = Vector3(0.0f, 0.0f, 1.0f);
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
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

bool loadConfig(Config& c) {
	FILE* f = fopen("plik.rtc", "r");
	if (!f) {
		return false;
	}

	char buffor[1000] = { 0 };
	while (fgets(buffor, 1000, f) && !feof(f)) {
		sscanf(buffor, "input %500s", c.input);
		sscanf(buffor, "output %500s", c.output);
		sscanf(buffor, "k %u", &c.k);
		sscanf(buffor, "xres %u", &c.xres);
		sscanf(buffor, "yres %u", &c.yres);
		sscanf(buffor, "VPx %f", &c.viewPoint.x);
		sscanf(buffor, "VPy %f", &c.viewPoint.y);
		sscanf(buffor, "VPz %f", &c.viewPoint.z);
		sscanf(buffor, "LAx %f", &c.lookAt.x);
		sscanf(buffor, "LAy %f", &c.lookAt.y);
		sscanf(buffor, "LAz %f", &c.lookAt.z);
		sscanf(buffor, "yview %f", &c.yview);
	}

	replace(&c.input[0], &c.input[500], '\\', '/');
	replace(&c.output[0], &c.output[500], '\\', '/');

	fclose(f);
	return true;
}
