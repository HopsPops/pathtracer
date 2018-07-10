#include <3dtree.hpp>
#include <assert.h>
#include <intersection.hpp>
#include <math.hpp>
#include <vertex.hpp>
#include <algorithm>
#include <random>

using namespace std;
typedef pair<PartitionData, SplitData> PartitionResult;

SplitData splitHeuristic(const Triangles& triangles, int axis, float splitPoint) {
	SplitData result { (int) triangles.size(), splitPoint };

	for (const Triangle* t : triangles) {
		result.min = fmin(result.min, t->minAxis(axis));
		result.max = fmax(result.max, t->maxAxis(axis));

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

PartitionData partitionAxis(const Triangles& triangles, int axis, float separator) {
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
	return PartitionData { triangles, left, right };
}

float calculateCost(SplitData split) {
	float leftShare = split.leftShare();

	assert(leftShare >= 0.0f && leftShare <= 1.0f);

	return (leftShare) * split.trianglesLeft * split.trianglesLeft + (1.0f - leftShare) * split.trianglesRight * split.trianglesRight;
}

PartitionResult partitionSAH(const Triangles& triangles, int axis) {
	SplitData bestSplit = splitHeuristic(triangles, axis, triangles[0]->v1.position[axis]);

	mt19937 gen(triangles.size());
	uniform_int_distribution<> distribution(0, triangles.size() - 1);

	for (int t = 0; t < fmin(300, (int) triangles.size()); t++) {
		const Triangle triangle = *triangles[distribution(gen)];

		SplitData s = splitHeuristic(triangles, axis, triangle.v1.position[axis]);

		if (calculateCost(bestSplit) > calculateCost(s)) {
			bestSplit = s;
		}
	}

	PartitionData partitionResult = partitionAxis(triangles, axis, bestSplit.splitPoint);
	PartitionResult result(partitionResult, bestSplit);

	return result;
}

int chooseAxis(const Triangles& triangles, int previousAxis) {
//	AABB aabb(triangles);
//
//	float lengthX = aabb.getMax().x - aabb.getMin().x;
//	float lengthY = aabb.getMax().y - aabb.getMin().y;
//	float lengthZ = aabb.getMax().z - aabb.getMin().z;
//
//	vector<pair<int, float>> lengths { make_pair(0, lengthX), make_pair(1, lengthY), make_pair(2, lengthZ) };
//
//	sort(lengths.begin(), lengths.end(), [](auto &left, auto &right) {
//		return left.second > right.second;
//	});
//
//	return lengths[0].first;
	return previousAxis + 1;
}

PartitionResult partitionMedian(const Triangles& triangles, int axis) {
	float splitPoint = trianglesMedian(triangles, axis);
	SplitData heuristics = splitHeuristic(triangles, axis, splitPoint);
	PartitionData partitionResult = partitionAxis(triangles, axis, splitPoint);
	return PartitionResult(partitionResult, heuristics);
}

KdTree::KdTree(const Triangles& triangles, int maxDepth, int depth, int axis) {
	this->axis = axis;
	this->aabb = AABB(triangles);

	if ((depth <= maxDepth) && (triangles.size() > 64)) {
		PartitionResult splited = partitionMedian(triangles, this->axis);
		SplitData heuristics = splited.second;
		PartitionData data = splited.first;
		this->splitPoint = heuristics.splitPoint;

//				if (data.maxWeight() > 0.85f) {
//					cout << triangles.size() << " left: " << splited.left.size() << " right: " << splited.right.size() << " ";
//					cout << "min: " << splited.split.min << " max: " << splited.split.max << " split: " << splited.split.splitPoint << " leftshare" << splited.split.leftShare() << " axis: " << this->axis << endl;
//					this->triangles = triangles;
//				} else {
		this->left = new KdTree(data.left, maxDepth, depth + 1, chooseAxis(data.left, axis));
		this->right = new KdTree(data.right, maxDepth, depth + 1, chooseAxis(data.right, axis));
//				}
	} else {
		this->triangles = triangles;
	}
}

bool KdTree::isLeaf() const {
	assert(((left == nullptr) ^ (right == nullptr)) == 0);
	return left == nullptr || right == nullptr;
}

int KdTree::getAxis() const {
	return axis % 3;
}

const KdTree* KdTree::getLeft() const {
	return left;
}

const KdTree* KdTree::getRight() const {
	return right;
}

const AABB& KdTree::getAABB() const {
	return aabb;
}

const Triangles& KdTree::getTriangles() const {
	return triangles;
}

unsigned int depth(const KdTree* tree, unsigned int level) {
	if (tree->isLeaf()) {
		return level + 1;
	} else {
		return max(depth(tree->getLeft(), level + 1), depth(tree->getRight(), level + 1));
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
	return (double) countTriangles(tree) / countLeafs(tree);
}

bool find(const KdTree* tree, const Ray& ray, long exclude, float* d, KdTreeTraversalResult* result, KdTreeDebugData* debug) {
	float tmin = INFINITY;
	float tmax = -INFINITY;

	if (!intersectAABB(ray, tree->getAABB(), tmin, tmax)) {
		return false;
	}

	if (tree->isLeaf()) {

		if (debug) {
			debug->traversedLeafs.push_back(tree);
		}

		bool found = false;
		for (const Triangle* triangle : tree->getTriangles()) {
			if (exclude == triangle->id) {
				continue;
			}

			unique_ptr<Vector3> intersection = intersectTriangle(ray, *triangle);
			if (!intersection) {
				continue;
			}

			if (debug) {
				debug->intersectedTriangles.push_back(triangle);
			}

			float dist = Vector3::distance(ray.getOrigin(), *intersection);

			if (dist < *d) {
				result->intersectionPoint = unique_ptr<Vector3>(intersection.release());
				result->intersectedTriangle = triangle;
				*d = dist;
				found = true;

				if (debug) {
					debug->leaf = tree;
				}
			}
		}
		return found;
	} else {
		const KdTree* n = tree->getLeft();
		const KdTree* f = tree->getRight();
		if (ray.getOrigin()[tree->getAxis()] > tree->splitPoint) {
			swap(n, f);
		}

		float tsplit = (tree->splitPoint - ray.getOrigin()[tree->getAxis()]) / ray.getDirection()[tree->getAxis()];
		if (tsplit > tmax || tsplit < 0) {
			return find(n, ray, exclude, d, result, debug);
		} else if (tsplit < tmin) {
			return find(f, ray, exclude, d, result, debug);
		} else {
			bool found = find(n, ray, exclude, d, result, debug);
			if (*d < tsplit) {
				return found;
			}
			return found | find(f, ray, exclude, d, result, debug);
		}
	}
}

bool find(const KdTree* tree, const Ray& ray, long exclude, KdTreeTraversalResult* result, KdTreeDebugData* debug) {
	float d = numeric_limits<float>::infinity();
	return find(tree, ray, exclude, &d, result, debug);
}
