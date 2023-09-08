#include "MoveCommand.h"
#include "Rand.h"

void MoveCommand::SetMoveCommand(int32_t setNum) {

	//行動を設定する
	auto itr = std::next(selectCommands_.begin(), setNum);
	moveCommands_.push_back(*itr);

}

void MoveCommand::PopBackMoveCommand() {

	//要素を後ろから取り出す
	moveCommands_.pop_back();

}

void MoveCommand::PopFrontMoveCommand() {

	//要素を前から取り出す
	moveCommands_.pop_front();

}

void MoveCommand::PopSelectCommand(int32_t num) {

	auto itr = selectCommands_.begin();
	std::advance(itr, num);
	selectCommands_.erase(itr);


}

void MoveCommand::PushSelectCommand(Command command) {

	selectCommands_.push_back(command);

}

void MoveCommand::ChangeCommand(int32_t num1, int32_t num2) {

	//入れ替えたい要素のイテレータを取得
	auto itr1 = std::next(moveCommands_.begin(), num1);
	auto itr2 = std::next(moveCommands_.begin(), num2);

	//要素を入れ替える
	moveCommands_.splice(itr1, moveCommands_, itr2);

}

void MoveCommand::SetSelectCommands() {

	//次の選択コマンドが空の場合、最初に設定しておく
	if (nextSelectCommands_.empty()) {

		//移動パターンを三種類設定
		for (int32_t i = 0; i < 3; i++) {

			Command command;

			command = Command(rand() % (kMovePattern - 3));

			nextSelectCommands_.push_back(command);
		}

		//攻撃パターンを二種類設定
		for (int32_t i = 0; i < 2; i++) {

			Command command;

			command = Command(rand() % (kMovePattern - 6) + 5);

			nextSelectCommands_.push_back(command);
		}

		//ガードを一つ設定
		nextSelectCommands_.push_back(Guard);

	}

	selectCommands_ = nextSelectCommands_;

	nextSelectCommands_.clear();

	// 移動パターンを三種類設定
	for (int32_t i = 0; i < 3; i++) {

		Command command;

		command = Command(rand() % (kMovePattern - 3));

		nextSelectCommands_.push_back(command);
	}

	// 攻撃パターンを二種類設定
	for (int32_t i = 0; i < 2; i++) {

		Command command;

		command = Command(rand() % (kMovePattern - 6) + 5);

		nextSelectCommands_.push_back(command);
	}

	// ガードを一つ設定
	nextSelectCommands_.push_back(Guard);

}
