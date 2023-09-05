#pragma once
#include <Vector3.h>

class Collider {
public:

	void SetGrid(int x, int y) {
		gridX_ = x;
		gridY_ = y;
	}

	int GetGridX() { return gridX_; }

	int GetGridY() { return gridY_; }

private:

	int gridX_;
	int gridY_;

};
