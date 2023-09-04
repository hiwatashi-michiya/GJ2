#pragma once
#include <list>

enum Command {
	MoveLeft,
	MoveRight,
	Jump,
	Attack,
	Guard,
	Stop
};

//行動パターンの種類
const int32_t kMovePattern = 6;

class MoveCommand {
public:

	/// <summary>
	/// 行動コマンドを設定する
	/// </summary>
	/// <param name="setNum">設定する数(行動数)</param>
	void SetMoveCommand(int32_t setNum);

	void PopMoveCommand();

	void ChangeCommand(int32_t num1, int32_t num2);

	Command GetNextCommand() { return moveCommands_.front(); }

	std::list<Command> GetmoveCommands() { return moveCommands_; }

private:

	std::list<Command> moveCommands_;

};
