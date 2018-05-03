#pragma once
#include "aabb.hpp"
#include "mesh.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include <vector>
#include <tuple>
#include <stdio.h>

enum Axis {
	X = 0, Y = 1, Z = 2
};

typedef std::vector<Triangle> Triangles;

class Modular {
	private:
		int state = 0;
		unsigned int modulus = 1;

	public:
		Modular(int digit, unsigned int modulus) {
			this->state = digit % modulus;
			this->modulus = modulus;
		}

//		Modular

		Modular& operator++() {
			state = (state + 1) % modulus;
			return *this;
		}

//		Modular& operator++(int) {
//			int old = state;
//			state = (state + 1) % modulus;
//			return *this;
//		}

		Modular operator+(int d) {
			Modular result((this->state + d) % modulus, modulus);
			return result;
		}

		operator int() const {
			return state;
		}
};

Vec3 triagnlesMedian(Triangles trigs) {
	Vec3 median { 0.0f, 0.0f, 0.0f };
	for (Triangle t : trigs) {
		median.x += t.v1.position.x / (float) trigs.size() / 3.0;
		median.y += t.v1.position.y / (float) trigs.size() / 3.0;
		median.z += t.v1.position.z / (float) trigs.size() / 3.0;

		median.x += t.v2.position.x / (float) trigs.size() / 3.0;
		median.y += t.v2.position.y / (float) trigs.size() / 3.0;
		median.z += t.v2.position.z / (float) trigs.size() / 3.0;

		median.x += t.v2.position.x / (float) trigs.size() / 3.0;
		median.y += t.v2.position.y / (float) trigs.size() / 3.0;
		median.z += t.v2.position.z / (float) trigs.size() / 3.0;
	}
	return median;
}

std::tuple<Triangles, Triangles> partitionX(Triangles triangles, Vec3 separator) {
	Triangles left;
	Triangles right;
	for (Triangle t : triangles) {
		bool pushLeft = false;
		bool pushRight = false;
		if (t.v1.position.x < separator.x) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (t.v2.position.x < separator.x) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (t.v3.position.x < separator.x) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (pushLeft) {
			left.push_back(t);
		}
		if (pushRight) {
			right.push_back(t);
		}
	}
	return std::make_tuple(left, right);
}
std::tuple<Triangles, Triangles> partitionY(Triangles triangles, Vec3 separator) {
	Triangles left;
	Triangles right;
	for (Triangle t : triangles) {
		bool pushLeft = false;
		bool pushRight = false;
		if (t.v1.position.y < separator.y) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (t.v2.position.y < separator.y) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (t.v3.position.y < separator.y) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (pushLeft) {
			left.push_back(t);
		}
		if (pushRight) {
			right.push_back(t);
		}
	}
	return std::make_tuple(left, right);
}
std::tuple<Triangles, Triangles> partitionZ(Triangles triangles, Vec3 separator) {
	Triangles left;
	Triangles right;
	for (Triangle t : triangles) {
		bool pushLeft = false;
		bool pushRight = false;
		if (t.v1.position.z < separator.z) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (t.v2.position.z < separator.z) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (t.v3.position.z < separator.z) {
			pushLeft = true;
		} else {
			pushRight = true;
		}
		if (pushLeft) {
			left.push_back(t);
		}
		if (pushRight) {
			right.push_back(t);
		}
	}
	return std::make_tuple(left, right);
}

std::tuple<Triangles, Triangles> partition(Triangles triangles, Vec3 separator, Modular axis) {
	if (axis == X) {
		return partitionX(triangles, separator);
	} else if (axis == Y) {
		return partitionY(triangles, separator);
	} else if (axis == Z) {
		return partitionZ(triangles, separator);
	} else {
		throw std::invalid_argument("wrong enum");
	}
}

AABB trianglesAABB(Triangles triangles) {
	Vec3 minimum(100000.0f, 100000.0f, 100000.0f);
	Vec3 maximum(-100000.0f, -100000.0f, -100000.0f);
	for (Triangle t : triangles) {
		minimum.x = min(minimum.x, t.v1.position.x);
		minimum.y = min(minimum.y, t.v1.position.y);
		minimum.z = min(minimum.z, t.v1.position.z);
		maximum.x = max(maximum.x, t.v1.position.x);
		maximum.y = max(maximum.y, t.v1.position.y);
		maximum.z = max(maximum.z, t.v1.position.z);

		minimum.x = min(minimum.x, t.v2.position.x);
		minimum.y = min(minimum.y, t.v2.position.y);
		minimum.z = min(minimum.z, t.v2.position.z);
		maximum.x = max(maximum.x, t.v2.position.x);
		maximum.y = max(maximum.y, t.v2.position.y);
		maximum.z = max(maximum.z, t.v2.position.z);

		minimum.x = min(minimum.x, t.v3.position.x);
		minimum.y = min(minimum.y, t.v3.position.y);
		minimum.z = min(minimum.z, t.v3.position.z);
		maximum.x = max(maximum.x, t.v3.position.x);
		maximum.y = max(maximum.y, t.v3.position.y);
		maximum.z = max(maximum.z, t.v3.position.z);
	}
	return AABB(minimum, maximum);
}

class KdTree {

	public:
		Modular axis = Modular(0, 3);
		KdTree* left = nullptr;
		KdTree* right = nullptr;
		AABB aabb;
		std::vector<Triangle> triangles { };

		KdTree(Triangles triangles, Axis axis) {
			this->axis = Modular(axis, 3);
			this->aabb = trianglesAABB(triangles);
			if (triangles.size() > 128) {
//				printf("splitting %d\n", triangles.size());
				Vec3 median = triagnlesMedian(triangles);
				std::tuple<Triangles, Triangles> splited = partition(triangles, median, this->axis);
//				printf("left %d\n", std::get<0>(splited).size());
//				printf("right %d\n", std::get<1>(splited).size());

				this->left = new KdTree(std::get<0>(splited), static_cast<Axis>((int) (this->axis + 1)));
				this->right = new KdTree(std::get<1>(splited), static_cast<Axis>((int) (this->axis + 1)));
			} else {
//				printf("done %d\n", triangles.size());
				this->triangles = triangles;
			}
		}

		bool isLeaf() const {
			return left == nullptr || right == nullptr;
		}

//	void add(std::vector<Triangle> trigs) {
//
//	}

};

unsigned int depth(const KdTree* tree, unsigned int level) {
	if (tree->isLeaf()) {
		return level + 1;
	} else {
		return max(depth(tree->left, level + 1), depth(tree->right, level + 1));
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

void traverse(const Ray& ray, const KdTree* tree, vector<const KdTree*>& result) {
	if (intersectAABB(ray, tree->aabb)) {
		if (tree->isLeaf()) {
			result.push_back(tree);
		} else {
			traverse(ray, tree->left, result);
			traverse(ray, tree->right, result);
		}
	}
}

