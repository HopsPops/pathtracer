#pragma once

#include <aabb.hpp>
#include <triangle.hpp>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
#include <ray.hpp>

struct SplitData {
		int totalTriangles = 0;
		float splitPoint = 0.0f;
		int trianglesLeft = 0;
		int trianglesRight = 0;
		float min = std::numeric_limits<float>::infinity();
		float max = -std::numeric_limits<float>::infinity();

		int trianglesSum() {
			return trianglesLeft + trianglesRight;
		}

		float leftShare() {
			return (splitPoint - min) / (max - min);
		}
};

struct PartitionData {
		const Triangles& triangles;
		Triangles left { };
		Triangles right { };

		int partitionedSize() {
			return left.size() + right.size();
		}

		float maxWeight() {
			return fmax(left.size() / (float) triangles.size(), right.size() / (float) triangles.size());
		}
};

typedef std::pair<PartitionData, SplitData> PartitionResult;

SplitData splitHeuristic(const Triangles& triangles, int axis, float splitPoint);

PartitionData partitionAxis(const Triangles& triangles, int axis, float separator);

float calculateCost(SplitData split);

PartitionResult partitionSAH(const Triangles& triangles, int axis);

int chooseAxis(const Triangles& triangles, int previousAxis);

PartitionResult partitionMedian(const Triangles& triangles, int axis);

class KdTree {

	private:
		int axis = 0;
		KdTree* left = nullptr;
		KdTree* right = nullptr;
		AABB aabb;
		Triangles triangles { };

	public:
		float splitPoint = 0.0f;

		KdTree(const Triangles& triangles, int maxDepth = 13, int depth = 0, int axis = 0);

		bool isLeaf() const;

		int getAxis() const;

		const KdTree* getLeft() const;

		const KdTree* getRight() const;

		const AABB& getAABB() const;

		const Triangles& getTriangles() const;

};

unsigned int depth(const KdTree* tree, unsigned int level);

unsigned int depth(const KdTree* tree);

unsigned int countLeafs(const KdTree* tree);

unsigned int countSubtrees(const KdTree* tree);

unsigned int countTriangles(const KdTree* tree);

double averageTrianglesPerLeaf(const KdTree* tree);

struct KdTreeTraversalResult {
		std::unique_ptr<Vector3> intersectionPoint { nullptr };
		const Triangle* intersectedTriangle = nullptr;

		bool didHit() {
			return (bool) intersectionPoint;
		}
};

struct KdTreeDebugData {
		const KdTree* leaf = nullptr;
		std::vector<const KdTree*> traversedLeafs { };
		std::vector<const Triangle*> intersectedTriangles { };

};

struct Entry {
		const KdTree* node = nullptr;
		float tmin, tmax;
};

bool find(const KdTree* tree, const Ray& ray, long exclude, float* d, KdTreeTraversalResult* result, KdTreeDebugData* debug = nullptr);

bool find(const KdTree* tree, const Ray& ray, long exclude, KdTreeTraversalResult* result, KdTreeDebugData* debug = nullptr);

