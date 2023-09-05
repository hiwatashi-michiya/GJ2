#include "Player.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG


void Player::Initialize(Sprite* sprite) {

	input_ = Input::GetInstance();

	playerSprite_ = sprite;
	position_ = {640.0f, 500.0f, 0.0f};
	velocity_ = {0.0f, 0.0f, 0.0f};
	SetSpritePosition();

}

void Player::Update() {

#ifdef _DEBUG

	ImGui::Begin("State");
	ImGui::Text("current Command %d", currentMoveCommand_);
	ImGui::End();

#endif // _DEBUG


	XINPUT_STATE joyState;

	//リストの要素が空なら新たに設定する
	if (GetmoveCommands().empty()) {
		SetMoveCommand(3);
	}
	else {

		//リスト内が空でないなら行動開始
		if (GetmoveCommands().empty() == false) {

			//ゲームパッドの取得
			if (input_->GetJoystickState(0, joyState)) {

				if ((input_->TriggerKey(DIK_SPACE) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && isMove_ == false) {

					//次の行動コマンドを現在の行動コマンドに設定
					currentMoveCommand_ = GetNextCommand();
					//先頭の行動コマンドを削除
					PopMoveCommand();
					//行動開始フラグを立てる
					MoveTimer_ = kMoveTime;
					isMove_ = true;

				}

			}
			//キーボードの場合
			else {

				if (input_->TriggerKey(DIK_SPACE) && isMove_ == false) {

					// 次の行動コマンドを現在の行動コマンドに設定
					currentMoveCommand_ = GetNextCommand();
					// 先頭の行動コマンドを削除
					PopMoveCommand();
					// 行動開始フラグを立てる
					MoveTimer_ = kMoveTime;
					isMove_ = true;
				}

			}

		}

	}

	// 行動フラグが立っていたら行動開始
	if (isMove_) {
		Move(currentMoveCommand_);
	} else {
		
	}

}

void Player::Move(Command command) {

	switch (command) {
	case MoveLeft:

		if (MoveTimer_ == 60) {
			velocity_ = {-1.0f, 0.0f, 0.0f};
		}

		position_ += velocity_;
		SetSpritePosition();

		break;
	case MoveRight:

		if (MoveTimer_ == 60) {
				velocity_ = {1.0f, 0.0f, 0.0f};
		}

		position_ += velocity_;
		SetSpritePosition();

		break;
	case Jump:

		if (MoveTimer_ == 60) {
			velocity_ -= {0.0f, 15.0f, 0.0f};
		}

		velocity_ += {0.0f, 0.5f, 0.0f};
		position_ += velocity_;
		SetSpritePosition();

		break;
	case Attack:

		//赤に設定
		playerSprite_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});

		break;
	case Guard:

		// 青に設定
		playerSprite_->SetColor({0.0f, 0.0f, 1.0f, 1.0f});

		break;
	default:
	case Stop:

		// 緑に設定
		playerSprite_->SetColor({0.0f, 1.0f, 0.0f, 1.0f});

		break;
	}

	position_.x = Clamp(position_.x, 32.0f, 1248.0f);
	position_.y = Clamp(position_.y, 0.0f, 500.0f);
	SetSpritePosition();

	if (--MoveTimer_ <= 0) {
		velocity_ = {0.0f, 0.0f, 0.0f};
		playerSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		isMove_ = false;
	}

}

void Player::Draw() {

	playerSprite_->Draw();

}
