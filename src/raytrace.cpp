//
//
//void rt(Model* model, unsigned char* data, int h, int n, RaytraceStatistics* stats) {
//	const int w = window->width();
//
//	Mat4 modelMatrix = transformations();
//
//	for (int y = h; y < n; y++) {
//		for (int x = 0; x < w; x++) {
//			Ray ray = Ray::createRay(camera, x, y, w, window->height(), window->aspectRatio());
//			stats->rays++;
//
//			if (!intersectAABB(ray, multiply(modelMatrix, model->bounding.getMin()), multiply(modelMatrix, model->bounding.getMax()))) {
//				continue;
//			}
//
//			float depth = INFINITY;
//
//			for (Mesh &mesh : model->meshes) {
//				if (!intersectAABB(ray, multiply(modelMatrix, mesh.bounding.getMin()), multiply(modelMatrix, mesh.bounding.getMax()))) {
//					continue;
//				}
//
//				Vec3 intersection(0.0, 0.0, 0.0);
//				for (unsigned int i = 0; i < mesh.positions.size();) {
//					Vec3 v1 = multiply(modelMatrix, mesh.positions[i++]);
//					Vec3 v2 = multiply(modelMatrix, mesh.positions[i++]);
//					Vec3 v3 = multiply(modelMatrix, mesh.positions[i++]);
//
//					stats->tests++;
//
//					if (!intersectTriangle(ray, v1, v2, v3, intersection)) {
//						continue;
//					}
//
////					double dist = glm::distance(ray.getOrigin(), intersection);
////
////					if (dist >= depth) {
////						continue;
////					}
////					depth = dist;
//					depth = 1.0; //póki co i tak sprawdzamy tylko czy natrafia na obiekt wiêc nie musimy robic depth-testu
//					stats->intersections++;
//					goto escape;
//					// wychodzimy z podwójnej pêtli
//				}
//			}
//			escape:
//
//			if (depth < INFINITY) {
//				data[(x + y * w) * 3 + 0] = 255;
//				data[(x + y * w) * 3 + 1] = 0;
//				data[(x + y * w) * 3 + 2] = 0;
//			}
//		}
////		cout << " " << y << endl;
//	}
//}
//
//void raytrace(Model* model) {
//	clock_t begin = clock();
//
//	unsigned char data[window->width() * window->height() * 3] = { 0 };
//	const int q = std::thread::hardware_concurrency();
//	vector<thread> threads;
//	int h = window->height();
//
//	RaytraceStatistics* stats = new RaytraceStatistics();
//	stats->tests = 0;
//	stats->intersections = 0;
//
//	for (int i = 0; i < q; i++) {
//		int from = h / q * i;
//		int to = from + h / q;
////		cout << from << " " << to << endl;
//		threads.push_back(thread(rt, model, &data[0], from, to, stats));
//	}
//
//	for (auto& th : threads) {
//		th.join();
//	}
//
//	clock_t end = clock();
//	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//
//	cout << "seconds: " << elapsed_secs << endl;
//	cout << "total rays: " << stats->rays << endl;
//	cout << "total tests: " << stats->tests << endl;
//	cout << "total intersections: " << stats->intersections << endl;
//
//	delete stats;
//
//	unsigned error = lodepng::encode(string(config.output), data, window->width(), window->height(), LCT_RGB);
//	cout << error << " " << config.output << endl;
//}
