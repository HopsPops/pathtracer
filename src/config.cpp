#include <config.hpp>

void Config::print(std::ostream& stream) const {
	stream << "input: " << input << std::endl;
	stream << "output: " << output << std::endl;
	stream << "k: " << k << std::endl;
	stream << "n: " << n << std::endl;
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

bool loadConfig(char* file, Config& c) {
	FILE* f = fopen(file, "r");
	if (!f) {
		return false;
	}

	char buffor[2048] = { 0 };
	while (fgets(buffor, 2048, f) && !feof(f)) {
		sscanf(buffor, "input %500s", c.input);
		sscanf(buffor, "output %500s", c.output);
		sscanf(buffor, "k %u", &c.k);
		sscanf(buffor, "n %u", &c.n);
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

	fclose(f);
	return true;
}
