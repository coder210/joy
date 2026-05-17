//#include <vector>
//#include "Simplex.h"
//#include "Gjk.h"
//
//
//static Vec3
//Support(Shape* shapeA, Shape* shapeB, Vec3 direction) {
//	Vec3 aFurthestPoint = shapeA->FindFurthestPoint(direction);
//	Vec3 bFurthestPoint = shapeB->FindFurthestPoint(-direction);
//	return  aFurthestPoint - bFurthestPoint;
//}
//
//static bool
//SameDirection(const Vec3& direction, const Vec3& ao) {
//	return Vec3Dot(direction, ao) > FP::Zero();
//}
//
//static std::pair<std::vector<Normal>, size_t>
//GetFaceNormals(const std::vector<Vec3>& polytope, const std::vector<size_t>& faces) {
//	std::vector<Normal> normals;
//	size_t minTriangle = 0;
//	FP  minDistance = FLT_MAX;
//
//	for (size_t i = 0; i < faces.size(); i += 3) {
//		Vec3 a = polytope[faces[i]];
//		Vec3 b = polytope[faces[i + 1]];
//		Vec3 c = polytope[faces[i + 2]];
//		Vec3 ba = Vec3Normalize(b - a);
//		Vec3 ca = Vec3Normalize(c - a);
//		Vec3 normal = Vec3Normalize(Vec3Cross(ba, ca));
//		FP distance = Vec3Dot(normal, a);
//		if (distance < FP::Zero()) {
//			normal = -normal;
//			distance = -distance;
//		}
//
//		normals.emplace_back(Normal{ normal.x, normal.y, normal.z, distance });
//
//		if (distance < minDistance) {
//			minTriangle = i / 3;
//			minDistance = distance;
//		}
//	}
//
//	return { normals, minTriangle };
//}
//
//static void
//AddIfUniqueEdge(
//	std::vector<std::pair<size_t, size_t>>& edges,
//	const std::vector<size_t>& faces,
//	size_t a,
//	size_t b)
//{
//	auto reverse = std::find(                       //      0--<--3
//		edges.begin(),                              //     / \ B /   A: 2-0
//		edges.end(),                                //    / A \ /    B: 0-2
//		std::make_pair(faces[b], faces[a])          //   1-->--2
//	);
//
//	if (reverse != edges.end()) {
//		edges.erase(reverse);
//	}
//	else {
//		edges.emplace_back(faces[a], faces[b]);
//	}
//}
//
//static Contact
//EPA(const Simplex& simplex, Shape* shapeA, Shape* shapeB) {
//	std::vector<Vec3> polytope(simplex.begin(), simplex.end());
//	std::vector<size_t> faces = {
//		0, 1, 2, // <0,1> <1,2> <2, 0>
//		0, 3, 1, // <0,3> <3,1> <1,0>
//		0, 2, 3, // <0,2> <2,3> <3,0>
//		1, 3, 2  // <1,3> <3,2> <2,1>
//	};
//
//	auto faceNormals = GetFaceNormals(polytope, faces);
//	std::vector<Normal> normals = faceNormals.first;
//	size_t minFace = faceNormals.second;
//
//	Vec3 bakeMinNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
//	FP bakeMinDistance = normals[minFace].w;
//
//	Vec3 minNormal;
//	FP minDistance = FP::MaxValue();
//
//	while (minDistance == FP::MaxValue()) {
//		minNormal = { normals[minFace].x, normals[minFace].y, normals[minFace].z };
//		minDistance = normals[minFace].w;
//		Vec3 support = Support(shapeA, shapeB, minNormal);
//		FP sDistance = Vec3Dot(minNormal, support);
//		if (FP::Abs(sDistance - minDistance) > FP(0.001f)) {
//			minDistance = FP::MaxValue();
//			std::vector<std::pair<size_t, size_t>> uniqueEdges;
//
//			for (size_t i = 0; i < normals.size(); i++) {
//				// 此线比较两个平行平面的最短距离和原点，通过一个强制点（支撑顶点与当前面的点）。如果支撑点的距离较小，则结果将是一个凹面多面体，这是不需要的。
//				Vec3 direction = { normals[i].x, normals[i].y, normals[i].z };
//				size_t f = i * 3;
//				FP a = Vec3Dot(direction, support);
//				FP b = Vec3Dot(direction, polytope[faces[f]]);
//				if (a > b){
//					AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
//					AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
//					AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);
//
//					faces[f + 2] = faces.back(); faces.pop_back();
//					faces[f + 1] = faces.back(); faces.pop_back();
//					faces[f] = faces.back(); faces.pop_back();
//
//					normals[i] = normals.back(); // pop-erase
//					normals.pop_back();
//
//					i--;
//				}
//			}
//			std::vector<size_t> newFaces;
//			for (int i = 0; i < uniqueEdges.size(); i++) {
//				size_t edgeIndex1 = uniqueEdges[i].first;
//				size_t edgeIndex2 = uniqueEdges[i].second;
//				newFaces.push_back(edgeIndex1);
//				newFaces.push_back(edgeIndex2);
//				newFaces.push_back(polytope.size());
//			}
//
//			polytope.push_back(support);
//
//			auto newFaceNormals = GetFaceNormals(polytope, newFaces);
//			std::vector<Normal> newNormals = newFaceNormals.first;
//			size_t newMinFace = newFaceNormals.second;
//
//			FP oldMinDistance = FP::MaxValue();
//			for (size_t i = 0; i < normals.size(); i++) {
//				if (normals[i].w < oldMinDistance) {
//					oldMinDistance = normals[i].w;
//					minFace = i;
//				}
//			}
//
//			if (newNormals.size() <= 0)
//			{
//				minNormal = bakeMinNormal;
//				minDistance = bakeMinDistance;
//				break;
//			}
//
//			if (newNormals[newMinFace].w < oldMinDistance) {
//				minFace = newMinFace + normals.size();
//			}
//
//			faces.insert(faces.end(), newFaces.begin(), newFaces.end());
//			normals.insert(normals.end(), newNormals.begin(), newNormals.end());
//		}
//	}
//
//	Contact contact;
//	contact.Normal = minNormal;
//	contact.Depth = minDistance + (FP)0.001f;
//	return contact;
//}
//
//
//static bool
//Line(Simplex& points, Vec3& direction) {
//	Vec3 a = points[0];
//	Vec3 b = points[1];
//
//	Vec3 ab = Vec3Normalize(b - a);
//	Vec3 ao = Vec3Normalize(-a);
//
//	if (SameDirection(ab, ao)) {
//		direction = Vec3Normalize(Vec3Cross(Vec3Cross(ab, ao), ab));
//	}
//	else {
//		points = { a };
//		direction = ao;
//	}
//
//	return false;
//}
//
//static bool
//Triangle(Simplex& points, Vec3& direction) {
//	Vec3 a = points[0];
//	Vec3 b = points[1];
//	Vec3 c = points[2];
//
//	Vec3 ab = Vec3Normalize(b - a);
//	Vec3 ac = Vec3Normalize(c - a);
//	Vec3 ao = Vec3Normalize(-a);
//
//	Vec3 abc = Vec3Normalize(Vec3Cross(ab, ac));
//
//	if (SameDirection(Vec3Cross(abc, ac), ao)) {
//		if (SameDirection(ac, ao)) {
//			points = { a, c };
//			direction = Vec3Normalize(Vec3Cross(Vec3Cross(ac, ao), ac));
//		}
//		else {
//			return Line(points = { a, b }, direction);
//		}
//	}
//	else {
//		if (SameDirection(Vec3Cross(ab, abc), ao)) {
//			return Line(points = { a, b }, direction);
//		}
//		else {
//			if (SameDirection(abc, ao)) {
//				direction = abc;
//			}
//			else {
//				points = { a, c, b };
//				direction = -abc;
//			}
//		}
//	}
//	return false;
//}
//
//static bool
//Tetrahedron(Simplex& points, Vec3& direction) {
//	Vec3 a = points[0];
//	Vec3 b = points[1];
//	Vec3 c = points[2];
//	Vec3 d = points[3];
//
//	Vec3 ab = Vec3Normalize(b - a);
//	Vec3 ac = Vec3Normalize(c - a);
//	Vec3 ad = Vec3Normalize(d - a);
//	Vec3 ao = Vec3Normalize(-a);
//
//	Vec3 abc = Vec3Normalize(Vec3Cross(ab, ac));
//	Vec3 acd = Vec3Normalize(Vec3Cross(ac, ad));
//	Vec3 adb = Vec3Normalize(Vec3Cross(ad, ab));
//
//	if (SameDirection(abc, ao)) {
//		return Triangle(points = { a, b, c }, direction);
//	}
//
//	if (SameDirection(acd, ao)) {
//		return Triangle(points = { a, c, d }, direction);
//	}
//
//	if (SameDirection(adb, ao)) {
//		return Triangle(points = { a, d, b }, direction);
//	}
//
//	return true;
//}
//
//static bool
//NextSimplex(Simplex& points, Vec3& direction) {
//	switch (points.size()) {
//	case 2: return Line(points, direction);
//	case 3: return Triangle(points, direction);
//	case 4: return Tetrahedron(points, direction);
//	}
//	// never should be here
//	return false;
//}
//
//
//bool
//Gjk(Shape* shapeA, Shape* shapeB, Vec3 initalDir, Contact* contect) {
//	// Get initial support point in any direction
//	Vec3 support = Support(shapeA, shapeB, initalDir);
//
//	Simplex points;
//
//	// Simplex is an array of points, max count is 4
//	points.push_front(support);
//
//	// New direction is towards the origin
//	Vec3 direction = Vec3Normalize(-support);
//	while (true) {
//		support = Support(shapeA, shapeB, direction);
//		if (Vec3Dot(support, direction) < FP::Zero()) {
//			return false; // no collision
//		}
//		points.push_front(support);
//		if (NextSimplex(points, direction)) {
//			*contect = EPA(points, shapeA, shapeB);
//			return true;
//		}
//	}
//}
//
//
//bool
//Gjk(Shape* shapeA, Shape* shapeB, Contact* contect) {
//	Vec3 initalDir = { FP::One(), 0.0f, 0.0f};
//	return Gjk(shapeA, shapeB, initalDir, contect);
//}
