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

enum Axis {
	X = 0, Y = 1, Z = 2
};

std::tuple<Triangles, Triangles> partitionAxis(const Triangles& triangles, Axis axis, double separator) {
	Triangles left;
	Triangles right;
	for (const Triangle* t : triangles) {
		if (t->v1.position[axis] <= separator || t->v2.position[axis] <= separator || t->v3.position[axis] <= separator) {
			left.push_back(t);
		}
		if (t->v1.position[axis] >= separator || t->v2.position[axis] >= separator || t->v3.position[axis] >= separator) {
			right.push_back(t);
		}
	}
	return std::make_tuple(left, right);
}

std::tuple<Triangles, Triangles> partition(const Triangles& triangles, Axis axis) {
	return partitionAxis(triangles, axis, trianglesMedian(triangles, (int) axis));
}

class KdTree {

	public:
		Axis axis = X;
		KdTree* left = nullptr;
		KdTree* right = nullptr;
		AABB aabb;
		Triangles triangles { };

		KdTree(const Triangles& triangles, Axis axis) {
			this->axis = axis;
			this->aabb = AABB(triangles);
//			Vector3 v = this->aabb.maximum - this->aabb.minimum;
//			this->aabb.maximum = this->aabb.maximum + (0.1 * v);
//			this->aabb.minimum = this->aabb.minimum - (0.1 * v);
			if (triangles.size() > 256) {
				std::tuple<Triangles, Triangles> splited = partition(triangles, this->axis);

				this->left = new KdTree(std::get<0>(splited), static_cast<Axis>((this->axis + 1) % 3));
				this->right = new KdTree(std::get<1>(splited), static_cast<Axis>((this->axis + 1) % 3));
			} else {
				this->triangles = triangles;
			}
		}

		bool isLeaf() const {
			return left == nullptr || right == nullptr;
		}

};

unsigned int depth(const KdTree* tree, unsigned int level) {
	if (tree->isLeaf()) {
		return level + 1;
	} else {
		return std::max(depth(tree->left, level + 1), depth(tree->right, level + 1));
	}
}

unsigned int depth(const KdTree* tree) {
	return depth(tree, 0);
}

unsigned int countLeafs(const KdTree* tree) {
	if (tree->isLeaf()) {
		return 1;
	} else {
		return countLeafs(tree->left) + countLeafs(tree->right);
	}
}

unsigned int countTriangles(const KdTree* tree) {
	if (tree->isLeaf()) {
		return tree->triangles.size();
	} else {
		return countTriangles(tree->left) + countTriangles(tree->right);
	}
}

double averageTrianglesPerLeaf(const KdTree* tree) {
	return (double) countTriangles(tree) / (double) countLeafs(tree);
}

bool find(const KdTree* tree, const Ray& ray, const Triangle* origin, Triangle* result, float* d, Vector3& intersectionOut, vector<AABB>* intersectedAABB) {
	if (intersectAABB(ray, tree->aabb)) {
		if (tree->isLeaf()) {

			if (intersectedAABB) {
				intersectedAABB->push_back(tree->aabb);
			}

			bool found = false;
			for (const Triangle* triangle : tree->triangles) {
				if (origin == triangle) {
					continue;
				}

				std::unique_ptr<Vector3> intersection = intersectTriangle(ray, *triangle);
				if (!intersection) {
					continue;
				}

				float dist = Vector3::distance(ray.getOrigin(), *intersection);

				if (dist < *d) {
					intersectionOut = *intersection;
					*result = *triangle;
					*d = dist;
					found = true;
				}
			}
			return found;
		} else {
			return find(tree->left, ray, origin, result, d, intersectionOut, intersectedAABB) ||
					find(tree->right, ray, origin, result, d, intersectionOut, intersectedAABB);
		}
	} else {
		return false;
	}
}

bool find(const KdTree* tree, const Ray& ray, const Triangle* origin, Triangle* result, Vector3& intersectionOut) {
	float d = INFINITY;
	return find(tree, ray, origin, result, &d, intersectionOut, nullptr);
}

