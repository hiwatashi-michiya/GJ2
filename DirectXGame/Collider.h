#pragma once
#include <Vector3.h>

class Collider {
public:

	void SetGrid(int x, int z) {
		gridX_ = x;
		gridZ_ = z;
	}

	int GetGridX() { return gridX_; }

	int GetGridZ() { return gridZ_; }

private:

	int gridX_;
	int gridZ_;

};
