#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector3& operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3& operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3& operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
};

// 直線
struct Line {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};

// 半直線
struct Ray {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};

// 線分
struct Segment {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};

// 三角形
struct Triangle {
	Vector3 vertices[3]; // 頂点
};

// AABB(箱)
struct AABB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};

// OBB(箱)
struct OBB {
	Vector3 center;          // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直交必須
	Vector3 size;            // 座標軸方向の長さの半分。中心から面までの距離
};

struct Matrix4x4;

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(float scalar, const Vector3& v1);

float Dot(const Vector3& v1, const Vector3& v2);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// ベクトル変換(スケールと回転)
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

// 球面線形補間
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

float Clamp(float x, float min, float max);

int IntClamp(int x, int min, int max);

Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

// 垂直なベクトルを求める関数
Vector3 Perpendicular(const Vector3& vector);

void ClampAABB(AABB& aabb);

// 2点間の距離を求める関数
float Distance(const Vector3& v1, const Vector3& v2);

// 二つの物体の半径から当たり判定を計算する関数
float HitRadius(float r1, float r2);
