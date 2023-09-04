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
	worldTransform_.Initialize();

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

	worldTransform_.UpdateMatrix();

}

void Player::Move(Command command) {

	switch (command) {
	case MoveLeft:

		velocity_ = {-1.0f / 6.0f, 0.0f, 0.0f};

		position_ += velocity_;
		worldTransform_.translation_ += velocity_;
		SetSpritePosition();

		break;
	case MoveRight:

		velocity_ = {1.0f / 6.0f, 0.0f, 0.0f};

		position_ += velocity_;
		worldTransform_.translation_ += velocity_;
		SetSpritePosition();

		break;
	case Jump:

		/*if (MoveTimer_ == 60) {
			velocity_ -= {0.0f, 15.0f, 0.0f};
		}

		velocity_ += {0.0f, 0.5f, 0.0f};*/

		position_ += velocity_;
		currentTex_ = textures_[0];
		SetSpritePosition();

		break;
	case Attack:

		velocity_ = {0.0f, 0.0f, 0.0f};

		//赤に設定
		playerSprite_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
		currentTex_ = textures_[1];

		break;
	case Guard:

		velocity_ = {0.0f, 0.0f, 0.0f};

		// 青に設定
		playerSprite_->SetColor({0.0f, 0.0f, 1.0f, 1.0f});
		currentTex_ = textures_[2];

		break;
	default:
	case Stop:

		velocity_ = {0.0f, 0.0f, 0.0f};

		// 緑に設定
		playerSprite_->SetColor({0.0f, 1.0f, 0.0f, 1.0f});
		currentTex_ = textures_[3];

		break;
	}

	position_.x = Clamp(position_.x, 32.0f, 1248.0f);
	position_.y = Clamp(position_.y, 0.0f, 500.0f);
	SetSpritePosition();

	if (--MoveTimer_ <= 0) {
		playerSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		velocity_.y = 0.0f;
		currentTex_ = textures_[0];
		isMove_ = false;
	}

}

void Player::Draw(const ViewProjection& viewProjection) {

	models_[0]->Draw(worldTransform_, viewProjection, currentTex_);

}
