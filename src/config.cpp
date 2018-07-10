#include <config.hpp>

using namespace std;

void Config::print(ostream& stream) const {
	stream << "input: " << input << endl;
	stream << "output: " << output << endl;
	stream << "k: " << k << endl;
	stream << "n: " << n << endl;
	stream << "xres: " << xres << endl;
	stream << "yres: " << xres << endl;
	stream << "viewPoint: " << viewPoint << endl;
	stream << "lookAt: " << lookAt << endl;
	stream << "up: " << up << endl;

//	stream << "VPx: " << viewPoint.x << endl;
//	stream << "VPy: " << viewPoint.y << endl;
//	stream << "VPz: " << viewPoint.z << endl;
//	stream << "LAx: " << lookAt.x << endl;
//	stream << "LAy: " << lookAt.y << endl;
//	stream << "LAz: " << lookAt.z << endl;
//	stream << "UPx: " << up.x << endl;
//	stream << "UPy: " << up.y << endl;
//	stream << "UPz: " << up.z << endl;
//	stream << "yview: " << yview << endl;
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
