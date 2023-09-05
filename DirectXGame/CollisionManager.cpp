#include "CollisionManager.h"

CollisionManager* CollisionManager::GetInstance() {
	
	static CollisionManager instance;
	return &instance;

}

void CollisionManager::SetCollision(int x, int z) {

	collisionMass_[z][x] = 1;

}

void CollisionManager::RemoveCollision(int x, int z) {

	collisionMass_[z][x] = 0;

}

bool CollisionManager::IsHit(int x, int z) {

	if (collisionMass_[z][x] == 0) {
		return false;
	}

	return true;

}
