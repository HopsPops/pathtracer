#include <3dtree.hpp>
#include <ext/type_traits.h>
#include <math.hpp>
#include <pathtracer.hpp>
#include <ray.hpp>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <ctime>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

using namespace std;

mt19937 genRussianRoulette(0);
uniform_real_distribution<float> russianRouletteDistribution(0.0f, 1.0f);

Vector3 shotRay(const KdTree* tree, const Triangles& lights, const Ray& ray, long exclude, int n, int k, TraverseDebug* debug) {
	if (n > k || (n > 10 && russianRouletteDistribution(genRussianRoulette) > 0.7f)) {
//		cout << "end " << n << " " << k << endl;
		return Vector3 { };
	}

	unique_ptr<KdTreeTraversalResult> traverseResult(new KdTreeTraversalResult);

	find(tree, ray, exclude, traverseResult.get());
	Vector3 result { };
	Vector3 directLight { };
	Vector3 indirectLight { };

	if (!traverseResult->didHit()) {
		return Vector3 { 0.75f, 0.7f, 0.7f }; //background
	} else {
		const Vector3 intersectionPoint = *traverseResult->intersectionPoint;
		const Triangle* intersectedTriangle = traverseResult->intersectedTriangle;
		Vector3 normal = intersectedTriangle->normal();

		Vector3 randomDirection = randomVectorOnHemisphere(normal);
//		if(Vector3::dot(normal, ray.getDirection()) > 0.0f) {
//			randomDirection.negate();
//		}
//		cout << "normal " << normal << endl;
		const Ray newRay { intersectionPoint, randomDirection };

		if (debug) {
			debug->lines.push_back(Line { ray.getOrigin(), intersectionPoint, Vector4 { 0.0f, 0.0f, 0.0f, 1.0f } });
		}

		if (intersectedTriangle->isLight()) {
			return intersectedTriangle->material->emmisive;
		} else {
			for (const Triangle* light : lights) {
				for (int i = 0; i < 5; i++) {
					Vector3 position = light->randomPointOnTriangle();
					Vector3 lightDir = (position - intersectionPoint).normalize();

					Ray lightRay { intersectionPoint, lightDir };

					unique_ptr<KdTreeTraversalResult> lightTraverseResult(new KdTreeTraversalResult);

					find(tree, lightRay, intersectedTriangle->id, lightTraverseResult.get());
					if (!lightTraverseResult->didHit()) {
						continue;
					}

					if (light->id == lightTraverseResult->intersectedTriangle->id) {
						directLight += fabs(Vector3::cosineAngle(light->normal(), lightDir)) * light->material->emmisive;

						if (debug) {
							debug->lines.push_back(Line { position, intersectionPoint, Vector4 { 1.0f, 1.0f, 0.0f, 1.0f } });
						}
					}
				}
				directLight = directLight / 5.0f;
			}
		}

		const float PDF = 1.0f / (2.0f * M_PI);
		const float THETA = fabs(Vector3::cosineAngle(normal, randomDirection));
		indirectLight = THETA / PDF * shotRay(tree, lights, newRay, intersectedTriangle->id, n + 1, k, debug);
		result = (directLight + 2.0f * indirectLight) / M_PI * intersectedTriangle->material->diffuse;
	}
	return result;
}

void pathtraceTask(const KdTree* tree, const Triangles& lights, const Camera& camera, unsigned char* data, atomic<int>* pixelCounter, mutex* mut, const TraceRequest& request) {
	const int totalPixels = request.width * request.height;

	mt19937 gen(0);
	uniform_real_distribution<float> dis(0.0f, 1.0f);

	while (*pixelCounter < totalPixels) {
		int currentPixel = pixelCounter->fetch_add(1);
		int x = currentPixel % request.width;
		int y = currentPixel / request.width;

		Vector3 color { };

		for (unsigned int i = 0; i < request.raysPerPixel; i++) {
			Ray ray = Ray::createRay(camera, x + dis(gen), y + dis(gen), request.width, request.height, (float) request.width / request.height);
			Vector3 result = shotRay(tree, lights, ray, -1, 0, request.k);
			color += (1.0f / request.raysPerPixel) * result.clamp(1.0f);
		}

		data[(x + y * request.width) * 3 + 0] = color.x * 255;
		data[(x + y * request.width) * 3 + 1] = color.y * 255;
		data[(x + y * request.width) * 3 + 2] = color.z * 255;

		if (((*pixelCounter % (totalPixels / 100)) == 0) && mut->try_lock()) {
			cout << (int) ((double) *pixelCounter / (request.height * request.width) * 100.0) << "%" << endl;
			mut->unlock();
		}
	}
}

void pathtrace(const KdTree* tree, const Triangles& triangles, const Camera& camera, unsigned char* data, const TraceRequest& request) {
	atomic<int> pixelCounter(0);
	mutex mut;
	const clock_t begin = clock();

	const Triangles lights = findLights(triangles);

	const int numberOfThreads = max(1, (int) thread::hardware_concurrency() - 1);
	vector<thread> threads { };

	for (int i = 0; i < numberOfThreads; i++) {
		threads.push_back(thread(pathtraceTask, tree, lights, camera, &data[0], &pixelCounter, &mut, request));
	}

	for (auto& th : threads) {
		th.join();
	}

	double elapsedSecs = double(clock() - begin) / CLOCKS_PER_SEC;
	cout << "seconds: " << elapsedSecs << endl;
}

void pathtrace(const KdTree* tree, const Triangles& triangles, const Camera& camera, unsigned char* data, const Config& config) {
	pathtrace(tree, triangles, camera, data, TraceRequest(config));
}
