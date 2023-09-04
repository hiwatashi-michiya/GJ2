#include "MoveCommand.h"
#include "Rand.h"

void MoveCommand::SetMoveCommand(int32_t setNum) {

	//行動をランダムに設定する
	for (int32_t i = 0; i < setNum; i++) {

		Command command;

		switch (rand() % kMovePattern) {
		default:
		case MoveLeft:

			command = MoveLeft;

			break;
		case MoveRight:

			command = MoveRight;

			break;
		case Jump:

			command = Jump;

			break;
		case Attack:

			command = Attack;

			break;
		case Guard:

			command = Guard;

			break;

		}

		moveCommands_.push_back(command);

	}

}

void MoveCommand::PopMoveCommand() {

	//要素を取り出す
	moveCommands_.pop_front();

}

void MoveCommand::ChangeCommand(int32_t num1, int32_t num2) {

	//入れ替えたい要素のイテレータを取得
	auto itr1 = std::next(moveCommands_.begin(), num1);
	auto itr2 = std::next(moveCommands_.begin(), num2);

	//要素を入れ替える
	moveCommands_.splice(itr1, moveCommands_, itr2);

}
