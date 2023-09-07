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

bool CollisionManager::IsHit(int x, int z, int num) {

	if (collisionMass_[z][x] == num) {
		return true;
	}

	return false;

}

void CollisionManager::SetAttackCross(int x, int z, int num) {

	for (int i = 1; i < 3; i++) {
		
		if (z - i >= 0) {
			collisionMass_[z - i][x] = num;
		}
		
		if (z + i < 6) {
			collisionMass_[z + i][x] = num;
		}

		if (x - i >= 0) {
			collisionMass_[z][x - i] = num;
		}

		if (x + i < 6) {
			collisionMass_[z][x + i] = num;
		}

	}

}

void CollisionManager::SetAttackCircle(int x, int z, int num) {

	if (z - 1 >= 0) {

		collisionMass_[z - 1][x] = num;

		if (x - 1 >= 0) {
			collisionMass_[z - 1][x - 1] = num;
		}

		if (x + 1 < 6) {
			collisionMass_[z - 1][x + 1] = num;
		}

	}

	if (z + 1 < 6) {

		collisionMass_[z + 1][x] = num;

		if (x - 1 >= 0) {
			collisionMass_[z + 1][x - 1] = num;
		}

		if (x + 1 < 6) {
			collisionMass_[z + 1][x + 1] = num;
		}

	}

	if (x - 1 >= 0) {
		collisionMass_[z][x - 1] = num;
	}

	if (x + 1 < 6) {
		collisionMass_[z][x + 1] = num;
	}
	
}
