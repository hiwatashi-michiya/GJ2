#include "Player.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG


void Player::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {

	input_ = Input::GetInstance();

	SetModels(models);
	SetTextures(textures);

	currentTex_ = textures_[0];

	for (int i = 0; i < kMaxCommand; i++) {
		commandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		commandNumSprite_[i]->SetSize({32.0f, 64.0f});
		commandNumSprite_[i]->SetTextureRect({0.0f,0.0f,}, {32.0f,64.0f});
		commandNumSprite_[i]->SetPosition({10.0f, (kMaxCommand - 1 - i) * 64.0f + 10.0f});

	}

	velocity_ = {0.0f, 0.0f, 0.0f};
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
	if (moveCommands_.empty()) {
		SetMoveCommand(kMaxCommand);
		UpdateMoveCommandsNum();
	}
	else {

		//リスト内が空でないなら行動開始
		if (moveCommands_.empty() == false) {

			//ゲームパッドの取得
			if (input_->GetJoystickState(0, joyState)) {

				if ((input_->TriggerKey(DIK_SPACE) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && isMove_ == false) {

					//次の行動コマンドを現在の行動コマンドに設定
					currentMoveCommand_ = GetNextCommand();
					//先頭の行動コマンドを削除
					PopMoveCommand();
					//数字更新
					UpdateMoveCommandsNum();
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
					// 数字更新
					UpdateMoveCommandsNum();
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

		worldTransform_.translation_ += velocity_;

		break;
	case MoveRight:

		velocity_ = {1.0f / 6.0f, 0.0f, 0.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveUp:

		velocity_ = {0.0f, 0.0f, 1.0f / 6.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveDown:

		velocity_ = {0.0f, 0.0f, -1.0f / 6.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case Jump:

		currentTex_ = textures_[0];

		break;
	case Attack:

		velocity_ = {0.0f, 0.0f, 0.0f};

		currentTex_ = textures_[1];

		break;
	case Guard:

		velocity_ = {0.0f, 0.0f, 0.0f};

		currentTex_ = textures_[2];

		break;
	default:
	case Stop:

		velocity_ = {0.0f, 0.0f, 0.0f};

		currentTex_ = textures_[3];

		break;
	}

	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -50.0f, 50.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -50.0f, 50.0f);

	if (--MoveTimer_ <= 0) {
		velocity_.y = 0.0f;
		currentTex_ = textures_[0];
		isMove_ = false;
	}

}

void Player::Draw(const ViewProjection& viewProjection) {

	models_[0]->Draw(worldTransform_, viewProjection, currentTex_);

}

void Player::DrawUI() {

	for (int i = 0; i < GetmoveCommands().size(); i++) {
		commandNumSprite_[i]->Draw();
	}

}

void Player::UpdateMoveCommandsNum() {

	for (int i = 0; i < moveCommands_.size(); i++) {

		auto itr = moveCommands_.begin();

		for (int k = 0; k < i; k++) {
			itr++;
		}

		int num = *itr;

		commandNumSprite_[i]->SetTextureRect({num * 32.0f, 0.0f}, {32.0f, 64.0f});
	}

}
