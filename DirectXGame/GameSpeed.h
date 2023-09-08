#pragma once
#include <Vector3.h>


class GameSpeed {
public:

	static GameSpeed* GetInstance();

	void SetGameSpeed(int speed) {

		speed = IntClamp(speed, 1, 3);

		gameSpeed_ = speed;

	}

	int GetGameSpeed() { return gameSpeed_; }

private:

	int gameSpeed_ = 2;

private:

	GameSpeed() = default;
	~GameSpeed() = default;
	GameSpeed(const GameSpeed&) = delete;
	const GameSpeed& operator=(const GameSpeed&) = delete;

};
