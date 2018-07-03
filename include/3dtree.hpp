#pragma once

#include <aabb.hpp>
#include <intersection.hpp>
#include <math.hpp>
#include <ray.hpp>
#include <triangle.hpp>
#include <vertex.hpp>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <set>
#include <assert.h>
#include <random>

enum Axis {
	X = 0, Y = 1, Z = 2
};

struct Split {
		int totalTriangles = 0;
		float splitPoint = 0.0f;
		int trianglesLeft = 0;
		int trianglesRight = 0;
		float min = INFINITE;
		float max = -INFINITE;

		int trianglesSum() {
			return trianglesLeft + trianglesRight;
		}

		float leftShare() {
			return (splitPoint - min) / (max - min);
		}
};

struct PartitionResult {
		const Triangles& triangles;
		Triangles left { };
		Triangles right { };
		Split split { };

		int partitionedSize() {
			return left.size() + right.size();
		}

		float maxWeight() {
			return max(left.size() / (float) triangles.size(), right.size() / (float) triangles.size());
		}
};

Split splitHeuristic(const Triangles& triangles, Axis axis, float splitPoint) {
	Split result { (int) triangles.size(), splitPoint };

	for (const Triangle* t : triangles) {
		result.min = min(result.min, t->minAxis(axis));
		result.max = max(result.max, t->maxAxis(axis));
		for (int i = 0; i < 3; i++) {
			if ((*t)[i].position[axis] <= splitPoint) {
				result.trianglesLeft++;
				continue;
			}
		}
		for (int i = 0; i < 3; i++) {
			if ((*t)[i].position[axis] >= splitPoint) {
				result.trianglesRight++;
				continue;
			}
		}
	}
	return result;
}

PartitionResult partitionAxis(const Triangles& triangles, Axis axis, float separator) {
	Triangles left { };
	Triangles right { };
	for (const Triangle* t : triangles) {
		if (t->v1.position[axis] <= separator || t->v2.position[axis] <= separator || t->v3.position[axis] <= separator) {
			left.push_back(t);
		}
		if (t->v1.position[axis] >= separator || t->v2.position[axis] >= separator || t->v3.position[axis] >= separator) {
			right.push_back(t);
		}
	}
	return PartitionResult { triangles, left, right };
}

float calculateCost(Split split) {
	float leftShare = split.leftShare();

	assert(leftShare >= 0.0f && leftShare <= 1.0f);

	return (leftShare) * split.trianglesLeft * split.trianglesLeft + (1.0f - leftShare) * split.trianglesRight * split.trianglesRight;
}

PartitionResult partition(const Triangles& triangles, Axis axis) {
//	cout << "initial split point " << triangles[0]->v1.position[axis] << endl;
	Split bestSplit = splitHeuristic(triangles, axis, triangles[0]->v1.position[axis]);

//	std::random_device rd;
	std::mt19937 gen(triangles.size());
	std::uniform_int_distribution<> distribution(0, triangles.size() - 1);

//	cout << triangles.size() << endl;

	for (int t = 0; t < min(300, (int) triangles.size()); t++) {
//	for (int t = 0; t < triangles.size(); t++) {
//		const Triangle triangle = *triangles[t];
		const Triangle triangle = *triangles[distribution(gen)];
//		for (int i = 0; i < 3; i++) {

			Split s = splitHeuristic(triangles, axis, (triangle).v1.position[axis]);

			if (calculateCost(bestSplit) > calculateCost(s)) {
				bestSplit = s;
			}
//		}
	}
//	cout << "best split point " << bestSplit.splitPoint << endl;
	PartitionResult result = partitionAxis(triangles, axis, bestSplit.splitPoint);
	result.split = bestSplit;
	return result;
}

Axis chooseAxis(const Triangles& triangles) {
	AABB aabb(triangles);

	float lengthX = aabb.getMax().x - aabb.getMin().x;
	float lengthY = aabb.getMax().y - aabb.getMin().y;
	float lengthZ = aabb.getMax().z - aabb.getMin().z;

	std::vector<std::pair<Axis, float>> lengths { make_pair(X, lengthX), make_pair(Y, lengthY), make_pair(Z, lengthZ) };

	std::sort(lengths.begin(), lengths.end(), [](auto &left, auto &right) {
		return left.second > right.second;
	});

	return lengths[0].first;
}

PartitionResult partitionMedian(const Triangles& triangles, Axis axis) {
	return partitionAxis(triangles, axis, trianglesMedian(triangles, (int) axis));
}

class KdTree {

	private:
		Axis axis = X;
		KdTree* left = nullptr;
		KdTree* right = nullptr;
		AABB aabb;
		Triangles triangles { };

	public:
		KdTree(const Triangles& triangles, Axis axis) {
			this->axis = chooseAxis(triangles);
			this->aabb = AABB(triangles);

			if (triangles.size() > 64) {
				PartitionResult splited = partitionMedian(triangles, this->axis);
				if (splited.maxWeight() > 0.85f) {
//					cout << triangles.size() << " left: " << splited.left.size() << " right: " << splited.right.size() << " ";
//					cout << "min: " << splited.split.min << " max: " << splited.split.max << " split: " << splited.split.splitPoint << " leftshare" << splited.split.leftShare() << " axis: " << this->axis << endl;
					this->triangles = triangles;
				} else {
					this->left = new KdTree(splited.left, static_cast<Axis>((this->axis + 1) % 3));
					this->right = new KdTree(splited.right, static_cast<Axis>((this->axis + 1) % 3));
				}
			} else {
//				cout << triangles.size() << endl;
				this->triangles = triangles;
			}
		}

		bool isLeaf() const {
			assert(((left == nullptr) ^ (right == nullptr)) == 0);
			return left == nullptr || right == nullptr;
		}

		Axis getAxis() const {
			return axis;
		}

		const KdTree* getLeft() const {
			return left;
		}

		const KdTree* getRight() const {
			return right;
		}

		const AABB& getAABB() const {
			return aabb;
		}

		const Triangles& getTriangles() const {
			return triangles;
		}

};

unsigned int depth(const KdTree* tree, unsigned int level) {
	if (tree->isLeaf()) {
		return level + 1;
	} else {
		return std::max(depth(tree->getLeft(), level + 1), depth(tree->getRight(), level + 1));
	}
}

unsigned int depth(const KdTree* tree) {
	return depth(tree, 0);
}

unsigned int countLeafs(const KdTree* tree) {
	if (tree->isLeaf()) {
		return 1;
	} else {
		return countLeafs(tree->getLeft()) + countLeafs(tree->getRight());
	}
}

unsigned int countSubtrees(const KdTree* tree) {
	if (tree->isLeaf()) {
		return 1;
	} else {
		return countSubtrees(tree->getLeft()) + countSubtrees(tree->getRight()) + 2;
	}
}

unsigned int countTriangles(const KdTree* tree) {
	if (tree->isLeaf()) {
		return tree->getTriangles().size();
	} else {
		return countTriangles(tree->getLeft()) + countTriangles(tree->getRight());
	}
}

double averageTrianglesPerLeaf(const KdTree* tree) {
	return (double) countTriangles(tree) / (double) countLeafs(tree);
}

class KdTreeTraversalResult {
	public:
		std::unique_ptr<Vector3> intersectionPoint { nullptr };
		const Triangle* intersectedTriangle = nullptr;
		const KdTree* leaf = nullptr;
		std::set<const KdTree*> traversedLeafs { };
		std::set<const Triangle*> intersectedTriangles { };

		bool didHit() {
			return (bool) intersectionPoint;
		}

};
bool find(const KdTree* tree, const Ray& ray, long exclude, KdTreeTraversalResult* result, double* d) {
	if (intersectAABB(ray, tree->getAABB())) {
		if (tree->isLeaf()) {
#ifdef DEBUG
			result->traversedLeafs.insert(tree);
#endif
			bool found = false;
			for (const Triangle* triangle : tree->getTriangles()) {
				if (exclude == triangle->id) {
					continue;
				}

				std::unique_ptr<Vector3> intersection = intersectTriangle(ray, *triangle);
				if (!intersection) {
					continue;
				}
#ifdef DEBUG
				result->intersectedTriangles.insert(intersectedTriangle);
#endif

				double dist = Vector3::distance(ray.getOrigin(), *intersection);

				if (dist < *d) {
					result->intersectionPoint = std::unique_ptr<Vector3>(intersection.release());
					result->intersectedTriangle = triangle;
					result->leaf = tree;
					*d = dist;
					found = true;
				}
			}
			return found;
		} else {
			return find(tree->getLeft(), ray, exclude, result, d) | find(tree->getRight(), ray, exclude, result, d);
		}

	} else {
		return false;
	}
}

bool find(const KdTree* tree, const Ray& ray, long exclude, KdTreeTraversalResult* result) {
	double d = INFINITY;
	return find(tree, ray, exclude, result, &d);
}

bool findAny(const KdTree* tree, const Ray& ray, long exclude, float maxDistance, KdTreeTraversalResult* result) {
	if (intersectAABB(ray, tree->getAABB())) {
		if (tree->isLeaf()) {

//#ifdef DEBUG
			result->traversedLeafs.insert(tree);
//#endif

			for (const Triangle* triangle : tree->getTriangles()) {
				if (triangle->id == exclude) {
					continue;
				}

				std::unique_ptr<Vector3> intersection = intersectTriangle(ray, *triangle);
				if (!intersection) {
					continue;
				}
				if (Vector3::distance(ray.getOrigin(), *intersection) > maxDistance) {
					continue;
				}
				result->intersectedTriangle = triangle;
				return true;
			}
			return false;
		} else {
			return findAny(tree->getLeft(), ray, exclude, maxDistance, result) || findAny(tree->getRight(), ray, exclude, maxDistance, result);
		}
	} else {
		return false;
	}
}

