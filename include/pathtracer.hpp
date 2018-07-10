#pragma once

#include <config.hpp>
#include <triangle.hpp>
#include <camera.hpp>

struct Line {
		Vector3 v1 { }, v2 { };
		Vector4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct Point {
		Vector3 position { };
		Vector4 color { };
};

struct TraverseDebug {
		std::vector<Line> lines;
};

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
Vector3 shotRay(const KdTree*, const Triangles&, const Ray&, long, int, int, TraverseDebug* = nullptr);

void pathtrace(const KdTree*, const Triangles&, const Camera&, unsigned char*, const TraceRequest&);
void pathtrace(const KdTree*, const Triangles&, const Camera&, unsigned char*, const Config&);
