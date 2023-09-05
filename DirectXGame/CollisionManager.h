#pragma once
#include "Collider.h"

const int kMaxGrid = 6;

class CollisionManager {
public:

	static CollisionManager* GetInstance();

	void SetCollision(int x, int z);

	void RemoveCollision(int x, int z);

	bool IsHit(int x, int z);

private:
	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	const CollisionManager& operator=(const CollisionManager&) = delete;

	int collisionMass_[kMaxGrid][kMaxGrid]{};

};
