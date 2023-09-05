#include "MoveCommand.h"
#include "Rand.h"

void MoveCommand::SetMoveCommand(int32_t setNum) {

	// 行動を設定する
	auto itr = std::next(selectCommands_.begin(), setNum);
	moveCommands_.push_back(*itr);
}

void MoveCommand::PopBackMoveCommand() {

	// 要素を後ろから取り出す
	moveCommands_.pop_back();
}

void MoveCommand::PopFrontMoveCommand() {

	// 要素を前から取り出す
	moveCommands_.pop_front();
}

void MoveCommand::PopSelectCommand(int32_t num) {

	auto itr = selectCommands_.begin();
	std::advance(itr, num);
	selectCommands_.erase(itr);
}

void MoveCommand::PushSelectCommand(Command command) { selectCommands_.push_back(command); }

void MoveCommand::ChangeCommand(int32_t num1, int32_t num2) {

	// 入れ替えたい要素のイテレータを取得
	auto itr1 = std::next(moveCommands_.begin(), num1);
	auto itr2 = std::next(moveCommands_.begin(), num2);

	// 要素を入れ替える
	moveCommands_.splice(itr1, moveCommands_, itr2);
}

void MoveCommand::SetSelectCommands(int32_t setNum) {

	// 次の選択コマンドが空の場合、最初に設定しておく
	if (nextSelectCommands_.empty()) {

		for (int32_t i = 0; i < setNum; i++) {

			Command command;

			command = Command(rand() % kMovePattern);

			nextSelectCommands_.push_back(command);
		}
	}

	selectCommands_ = nextSelectCommands_;

	nextSelectCommands_.clear();

	for (int32_t i = 0; i < setNum; i++) {

		Command command;

		command = Command(rand() % kMovePattern);

		nextSelectCommands_.push_back(command);
	}
}
