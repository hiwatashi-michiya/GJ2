#include "GameSpeed.h"

GameSpeed* GameSpeed::GetInstance() {

	static GameSpeed instance;
	return &instance;

}
