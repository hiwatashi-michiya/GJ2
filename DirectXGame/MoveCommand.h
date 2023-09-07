#pragma once
#include <list>

enum Command {
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	AttackCross,
	AttackCircle,
	Guard,
	Stop
};

//行動パターンの種類
const int32_t kMovePattern = 8;

class MoveCommand {
public:

	/// <summary>
	/// 行動コマンドを設定する
	/// </summary>
	/// <param name="setNum">設定する数(行動数)</param>
	void SetMoveCommand(int32_t setNum);

	void PopBackMoveCommand();

	void PopFrontMoveCommand();

	void PopSelectCommand(int32_t num);

	void PushSelectCommand(Command command);

	void ChangeCommand(int32_t num1, int32_t num2);

	void SetSelectCommands(int32_t setNum);

	Command GetNextCommand() { return moveCommands_.front(); }

	std::list<Command> GetmoveCommands() { return moveCommands_; }

	std::list<Command> moveCommands_;

	std::list<Command> selectCommands_;

	std::list<Command> nextSelectCommands_;

private:

	

};
