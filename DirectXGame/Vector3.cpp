#include <Matrix4x4.h>
#include <Vector3.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <math.h>

// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {

	Vector3 v;

	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	v.z = v1.z + v2.z;

	return v;
}

// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {

	Vector3 v;

	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	v.z = v1.z - v2.z;

	return v;
}

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v1) {

	Vector3 v;

	v.x = scalar * v1.x;
	v.y = scalar * v1.y;
	v.z = scalar * v1.z;

	return v;
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {

	float x;

	x = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return x;
}

// 長さ(ノルム)
float Length(const Vector3& v) {

	float length;

	length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	return length;
}

// 正規化
Vector3 Normalize(const Vector3& v) {

	float length = Length(v);

	Vector3 newV;

	newV.x = v.x;
	newV.y = v.y;
	newV.z = v.z;

	if (length != 0.0f) {

		newV.x = v.x / length;
		newV.y = v.y / length;
		newV.z = v.z / length;
	}

	return newV;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {

	Vector3 result = {};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] +
	           1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] +
	           1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] +
	           1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] +
	          1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

// ベクトル変換(スケールと回転)
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {

	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {

	Vector3 v(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);

	return v;
}

// 球面線形補間
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {

	t = Clamp(t, 0, 1.0f);

	float theta = acosf(Clamp(Dot(v1, v2), 0, 1.0f) / (Length(v1) * Length(v2)));

	float s = (1.0f - t) * Length(v1) + t * Length(v2);

	Vector3 p = Vector3(
	    s * (std::sin(1 - t) / std::sin(theta) * v1.x / Length(v1) +
	         std::sin(t) / std::sin(theta) * v2.x / Length(v2)),
	    s * (std::sin(1 - t) / std::sin(theta) * v1.y / Length(v1) +
	         std::sin(t) / std::sin(theta) * v2.y / Length(v2)),
	    s * (std::sin(1 - t) / std::sin(theta) * v1.z / Length(v1) +
	         std::sin(t) / std::sin(theta) * v2.z / Length(v2)));

	return p;
}

float Clamp(float x, float min, float max) {

	if (x > max) {
		return max;
	}

	if (x < min) {
		return min;
	}

	return x;
}

int IntClamp(int x, int min, int max) {

	if (x > max) {
		return max;
	}

	if (x < min) {
		return min;
	}

	return x;
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {

	Vector3 project;
	float lengthB = Length(v2);

	project = Multiply(Dot(v1, v2) / (lengthB * lengthB), v2);

	return project;
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {

	Vector3 cp;

	cp = Add(segment.origin, Project(Subtract(point, segment.origin), segment.diff));

	return cp;
}

Vector3 Perpendicular(const Vector3& vector) {

	if (vector.x != 0.0f || vector.y != 0.0f) {
		return {-vector.y, vector.x, 0.0f};
	}

	return {0.0f, -vector.z, vector.y};
}

void ClampAABB(AABB& aabb) {

	aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
	aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);
	aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
	aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);
	aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
	aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);
}

// 2点間の距離を求める
float Distance(const Vector3& v1, const Vector3& v2) {

	Vector3 v = Subtract(v2, v1);
	float result = Dot(v, v);

	return result;
}

float HitRadius(float r1, float r2) {

	float totalRadius = r1 + r2;

	float result = totalRadius * totalRadius;

	return result;
}
