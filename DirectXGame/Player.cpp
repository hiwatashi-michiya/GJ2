#include "Player.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG


void Player::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {

	input_ = Input::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();

	SetModels(models);
	SetTextures(textures);

	currentTex_ = textures_[0];

	for (int i = 0; i < kMaxCommand; i++) {
		commandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		commandNumSprite_[i]->SetSize({64.0f, 64.0f});
		commandNumSprite_[i]->SetTextureRect({0.0f,0.0f,}, {64.0f,64.0f});
		commandNumSprite_[i]->SetPosition({10.0f, (kMaxCommand - 1 - i) * 64.0f + 10.0f});

	}

	for (int i = 0; i < kMaxSelectNum; i++) {

		selectCommandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		selectCommandNumSprite_[i]->SetSize({64.0f, 64.0f});
		selectCommandNumSprite_[i]->SetTextureRect({0.0f,0.0f,}, {64.0f,64.0f});
		selectCommandNumSprite_[i]->SetPosition({i * 64.0f + 300.0f, 600.0f});

	}

	currentNumSprite_.reset(Sprite::Create(textures_[0], {0.0f, 0.0f}));
	currentNumSprite_->SetSize({16.0f, 16.0f});
	currentNumSprite_->SetPosition({selectNum_ * 64.0f + 316.0f, 550.0f});

	velocity_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Initialize();
	worldTransform_.translation_ = Vector3(-25.0f, 2.0f, 5.0f);
	worldTransform_.UpdateMatrix();

	for (int i = 0; i < 4; i++) {
		worldTransformEffect_[i].Initialize();
		worldTransformEffect_[i].scale_ *= 2.0f;
		worldTransformEffect_[i].parent_ = &worldTransform_;
		worldTransformEffect_[i].UpdateMatrix();
	}

	SetSelectCommands(kMaxSelectNum);
	SetGrid(0, 2);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

}

void Player::Update(Option* option) {

#ifdef _DEBUG

	ImGui::Begin("State");
	ImGui::Text("current Command %d", currentMoveCommand_);
	ImGui::End();

#endif // _DEBUG


	XINPUT_STATE joyState;

	if (inputCoolTimer_ > 0) {
		inputCoolTimer_--;
	}

	//リストの要素が空なら新たに設定する
	if (isSelect_) {

		if (input_->GetJoystickState(0, joyState)) {

			if ((input_->PushKey(DIK_LEFT) || option->GetActionTrigger(L_SELECT))
				&& inputCoolTimer_ == 0) {

				if (selectNum_ > 0) {
					selectNum_--;
				}

				inputCoolTimer_ = kInputCoolTime;

			}

			else if ((input_->PushKey(DIK_RIGHT) ||
			     option->GetActionTrigger(R_SELECT)) &&
			    inputCoolTimer_ == 0) {

				if (selectNum_ < selectCommands_.size() - 1) {
					selectNum_++;
				}

				inputCoolTimer_ = kInputCoolTime;

			}

			if ((input_->PushKey(DIK_E) || option->GetActionTrigger(ACT)) &&
				inputCoolTimer_ == 0 &&
				moveCommands_.size() < kMaxCommand) {

				SetMoveCommand(selectNum_);
				PopSelectCommand(selectNum_);

				inputCoolTimer_ = kInputCoolTime;

			} else if (
			    (input_->PushKey(DIK_E) || option->GetActionTrigger(ACT)) &&
				inputCoolTimer_ == 0) {
				isSelect_ = false;
				inputCoolTimer_ = kInputCoolTime;
			}

			if ((input_->PushKey(DIK_Q) || option->GetActionTrigger(CANCEL)) &&
				inputCoolTimer_ == 0 &&
				moveCommands_.size() != 0) {
				PushSelectCommand(moveCommands_.back());
				PopBackMoveCommand();
				inputCoolTimer_ = kInputCoolTime;
			}

			if (selectNum_ >= selectCommands_.size()) {
				selectNum_ = int(selectCommands_.size() - 1);
			}

		}



		UpdateMoveCommandsNum();
	}
	else {

		//リスト内が空でないなら行動開始
		if (moveCommands_.empty() == false) {

			if (isMove_ == false) {
				// 次の行動コマンドを現在の行動コマンドに設定
				currentMoveCommand_ = GetNextCommand();
				// 先頭の行動コマンドを削除
				PopFrontMoveCommand();
				// 数字更新
				UpdateMoveCommandsNum();
				// 行動開始フラグを立てる
				MoveTimer_ = kMoveTime;
				isMove_ = true;
			}

			////ゲームパッドの取得
			//if (input_->GetJoystickState(0, joyState)) {

			//	if ((input_->TriggerKey(DIK_SPACE) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && isMove_ == false) {

			//		//次の行動コマンドを現在の行動コマンドに設定
			//		currentMoveCommand_ = GetNextCommand();
			//		//先頭の行動コマンドを削除
			//		PopFrontMoveCommand();
			//		//数字更新
			//		UpdateMoveCommandsNum();
			//		//行動開始フラグを立てる
			//		MoveTimer_ = kMoveTime;
			//		isMove_ = true;

			//	}

			//}
			////キーボードの場合
			//else {

			//	if (input_->TriggerKey(DIK_SPACE) && isMove_ == false) {

			//		// 次の行動コマンドを現在の行動コマンドに設定
			//		currentMoveCommand_ = GetNextCommand();
			//		// 先頭の行動コマンドを削除
			//		PopFrontMoveCommand();
			//		// 数字更新
			//		UpdateMoveCommandsNum();
			//		// 行動開始フラグを立てる
			//		MoveTimer_ = kMoveTime;
			//		isMove_ = true;
			//	}

			//}

		}
		else {
			SetSelectCommands(kMaxSelectNum);
			isSelect_ = true;
		}

	}

	// 行動フラグが立っていたら行動開始
	if (isMove_) {
		Move(currentMoveCommand_);
	} else {
		currentMoveCommand_ = Stop;
	}

	worldTransform_.UpdateMatrix();
	for (int i = 0; i < 4; i++) {
		worldTransformEffect_[i].UpdateMatrix();
	}

}

void Player::Move(Command& command) {

	switch (command) {
	case MoveLeft:

		if (MoveTimer_ == 60) {

			int tmpX = GetGridX() - 1;
			int tmpZ = GetGridZ();

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			//障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);

		}

		velocity_ = {-1.0f / 6.0f, 0.0f, 0.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveRight:

		if (MoveTimer_ == 60) {

			int tmpX = GetGridX() + 1;
			int tmpZ = GetGridZ();

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);

		}

		velocity_ = {1.0f / 6.0f, 0.0f, 0.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveUp:

		if (MoveTimer_ == 60) {

			int tmpX = GetGridX();
			int tmpZ = GetGridZ() - 1;

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);

		}

		velocity_ = {0.0f, 0.0f, 1.0f / 6.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveDown:

		if (MoveTimer_ == 60) {

			int tmpX = GetGridX();
			int tmpZ = GetGridZ() + 1;

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);

		}

		velocity_ = {0.0f, 0.0f, -1.0f / 6.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case AttackCross:

		if (MoveTimer_ == 60) {
			
			for (int i = 0; i < 4; i++) {
				worldTransformEffect_[i].translation_ = Vector3(0.0f, 0.0f, 0.0f);
			}

		}

		if (MoveTimer_ % 5 == 0) {
			for (int i = 0; i < 4; i++) {
				worldTransformEffect_[i].translation_.y = -3.0f;
			}
		}

		for (int i = 0; i < 4; i++) {
			velocity_ = {cosf(i * 3.14f / 2.0f) / 3.0f,1.0f, sinf(i * 3.14f / 2.0f) / 3.0f};
			worldTransformEffect_[i].translation_ += velocity_;
		}

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

	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -25.0f, 25.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -25.0f, 25.0f);

	if (--MoveTimer_ <= 0) {
		velocity_.y = 0.0f;
		currentTex_ = textures_[0];
		isMove_ = false;
	}

}

void Player::Draw(const ViewProjection& viewProjection) {

	models_[0]->Draw(worldTransform_, viewProjection, currentTex_);

	if (currentMoveCommand_ == AttackCross) {

		for (int i = 0; i < 4; i++) {
			models_[1]->Draw(worldTransformEffect_[i], viewProjection, textures_[5]);
		}

	}

}

void Player::DrawUI() {

	for (int i = 0; i < GetmoveCommands().size(); i++) {
		commandNumSprite_[i]->Draw();
	}

	for (int i = 0; i < selectCommands_.size(); i++) {
		selectCommandNumSprite_[i]->Draw();
	}

	currentNumSprite_->Draw();

}

void Player::UpdateMoveCommandsNum() {

	for (int i = 0; i < moveCommands_.size(); i++) {

		auto itr = moveCommands_.begin();

		for (int k = 0; k < i; k++) {
			itr++;
		}

		int num = *itr;

		commandNumSprite_[i]->SetTextureRect({num * 64.0f, 0.0f}, {64.0f, 64.0f});
	}

	for (int i = 0; i < selectCommands_.size(); i++) {

		auto itr = selectCommands_.begin();

		for (int k = 0; k < i; k++) {
			itr++;
		}

		int num = *itr;

		selectCommandNumSprite_[i]->SetTextureRect({num * 64.0f, 0.0f}, {64.0f, 64.0f});

	}

	currentNumSprite_->SetPosition({selectNum_ * 64.0f + 316.0f, 550.0f});

}
