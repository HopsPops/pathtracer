#pragma once

#include <math.hpp>
#include <iostream>

class Config {
	public:
		char input[1024] = { 0 };
		char output[1024] = { 0 };
		unsigned int k = 0;
		unsigned int n = 50;
		unsigned int xres = 800;
		unsigned int yres = 600;
		Vector3 viewPoint { 0.0f, 0.0f, 0.0f };
		Vector3 lookAt { 0.0f, 0.0f, 1.0f };
		Vector3 up { 0.0f, 1.0f, 0.0f };
		float yview;

		void print(std::ostream& stream) const;

};

bool loadConfig(char* file, Config& c);
