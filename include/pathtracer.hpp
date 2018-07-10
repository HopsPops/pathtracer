#pragma once

#include <config.hpp>
#include <triangle.hpp>
#include <camera.hpp>

class TraceRequest {
	public:
		unsigned int width = 400;
		unsigned int height = 400;
		unsigned int raysPerPixel = 50;
		unsigned int k = 5;

		TraceRequest() {
		}

		TraceRequest(const Config& config) {
			this->width = config.xres;
			this->height = config.yres;
			this->raysPerPixel = config.n;
			this->k = config.k;
		}
};

void pathtrace(const KdTree*, const Triangles&, const Camera&, unsigned char*, const TraceRequest&);
void pathtrace(const KdTree*, const Triangles&, const Camera&, unsigned char*, const Config&);
