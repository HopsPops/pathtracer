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

enum Axis {
	X = 0, Y = 1, Z = 2
};

struct PartitionResult {
	Triangles left {};
	Triangles right {};
};

PartitionResult partitionAxis(const Triangles& triangles, Axis axis, double separator) {
	Triangles left {};
	Triangles right {};
	for (const Triangle* t : triangles) {
		if (t->v1.position[axis] <= separator || t->v2.position[axis] <= separator || t->v3.position[axis] <= separator) {
			left.push_back(t);
		}
		if (t->v1.position[axis] >= separator || t->v2.position[axis] >= separator || t->v3.position[axis] >= separator) {
			right.push_back(t);
		}
	}
	return PartitionResult {left, right};
}

PartitionResult partition(const Triangles& triangles, Axis axis) {
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
			this->axis = axis;
			this->aabb = AABB(triangles);

			if (triangles.size() > 32) {
				PartitionResult splited = partition(triangles, this->axis);
				this->left = new KdTree(splited.left, static_cast<Axis>((this->axis + 1) % 3));
				this->right = new KdTree(splited.right, static_cast<Axis>((this->axis + 1) % 3));
			} else {
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
		const Triangle* triangle = nullptr;
		const KdTree* leaf = nullptr;
		std::set<const KdTree*> traversedLeafs {};
		std::set<const Triangle*> intersectedTriangles {};

		bool didHit() {
			return (bool) intersectionPoint;
		}

};
bool find(const KdTree* tree, const Ray& ray, const Triangle* origin, KdTreeTraversalResult* result, double* d) {
	if (intersectAABB(ray, tree->getAABB())) {
		if (tree->isLeaf()) {
			result->traversedLeafs.insert(tree);
			bool found = false;
			for (const Triangle* triangle : tree->getTriangles()) {
				if (origin && (origin->id == triangle->id)) {
					continue;
				}

				std::unique_ptr<Vector3> intersection = intersectTriangle(ray, *triangle);
				if (!intersection) {
					continue;
				}
				result->intersectedTriangles.insert(triangle);

				double dist = Vector3::distance(ray.getOrigin(), *intersection);

				if (dist < *d) {
					result->intersectionPoint = std::unique_ptr<Vector3>(intersection.release());
					result->triangle = triangle;
					result->leaf = tree;
					*d = dist;
					found = true;
				}
			}
			return found;
		} else {
//			bool foundLeft = find(tree->getLeft(), ray, origin, result, d);
//			bool foundRight = find(tree->getRight(), ray, origin, result, d);
//			return foundLeft || foundRight;
			return find(tree->getLeft(), ray, origin, result, d) | find(tree->getRight(), ray, origin, result, d);
		}

	} else {
		return false;
	}
}

bool find(const KdTree* tree, const Ray& ray, const Triangle* origin, KdTreeTraversalResult* result) {
	double d = INFINITY;
	return find(tree, ray, origin, result, &d);
}

