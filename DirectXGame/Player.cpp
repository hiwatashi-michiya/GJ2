#include "Player.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

void Player::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {

	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();
	gameSpeed_ = GameSpeed::GetInstance();

	SetModels(models);
	SetTextures(textures);

	currentTex_ = textures_[0];
	findUITexture_ = TextureManager::Load("findUI.png");

	for (int i = 0; i < kMaxCommand; i++) {
		commandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		commandNumSprite_[i]->SetSize({64.0f, 64.0f});
		commandNumSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {1024.0f, 1024.0f});
		commandNumSprite_[i]->SetPosition({10.0f, (kMaxCommand - 1 - i) * 64.0f + 10.0f});
	}

	for (int i = 0; i < kMaxSelectNum; i++) {

		selectCommandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		selectCommandNumSprite_[i]->SetSize({64.0f, 64.0f});
		selectCommandNumSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {1024.0f, 1024.0f});
		selectCommandNumSprite_[i]->SetPosition({10.0f, i * 64.0f + 222.0f});
	}

	for (int i = 0; i < kMaxSelectNum; i++) {
		findUI_[i].reset(Sprite::Create(findUITexture_, {0.0f, 0.0f}));
		findUI_[i]->SetSize({64.0f, 64.0f});
		findUI_[i]->SetTextureRect({0.0f, 0.0f,},{32.0f, 32.0f});
		findUI_[i]->SetPosition({selectCommandNumSprite_[i]->GetPosition().x,selectCommandNumSprite_[i]->GetPosition().y});
	}

	for (int i = 0; i < 3; i++) {
		hpSprite_[i].reset(Sprite::Create(textures_[6], {0.0f, 0.0f}));
		hpSprite_[i]->SetSize({48.0f, 64.0f});
		hpSprite_[i]->SetTextureRect({0.0f,0.0f,},{1024.0f, 1024.0f});
		hpSprite_[i]->SetPosition({(float)(i + 1) * 48, 10.0f});
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

	SetSelectCommands();
	SetGrid(0, 2);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

	crossAttackSE_ = audio_->LoadWave("SE/crossattack.wav");
	upMoveSE_ = audio_->LoadWave("SE/up.wav");
	clickSE_ = audio_->LoadWave("SE/click.wav");
	selectSE_ = audio_->LoadWave("SE/select.wav");
	cancelSE_ = audio_->LoadWave("SE/cancel.wav");
}

void Player::Update(Option* option) {

#ifdef _DEBUG

	ImGui::Begin("State");
	ImGui::Text("HP %d", life_);
	ImGui::End();

	if (input_->TriggerKey(DIK_0)) {
		life_ -= 20;
	}

#endif // _DEBUG

	if (isHit_ == false) {

		if (collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), EnemyAttack)) {

			//ガード状態なら半減し、その後ガードを解除する
			if (isGuard_) {
				life_ -= 5;
				isGuard_ = false;
			}
			else {
				life_ -= 10;
			}

			isHit_ = true;

		}
		else if (collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), EnemySpecialAttack)) {

			// ガード状態なら半減し、その後ガードを解除する
			if (isGuard_) {
				life_ -= 10;
				isGuard_ = false;
			} else {
				life_ -= 20;
			}

			isHit_ = true;

		}

	}
	// アタックが終了したらヒットフラグを降ろす
	else if (isHit_ == true && collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), 0)) {

		isHit_ = false;
	}

	// 体力が0以下で死亡
	if (life_ <= 0) {
		collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
		isDead_ = true;
	}

	// ポインターを行動カードと接触してる状態でXボタンを押すと効果を表示
	for (int i = 0; i < kMaxSelectNum; i++) {
		Vector2 pos = selectCommandNumSprite_[i]->GetPosition();
		Vector2 size = selectCommandNumSprite_[i]->GetSize();

		if ((pos.x) <= (option->GetCursorPos().x + option->GetCursorRad().x) &&
		    (pos.x + size.x) >= (option->GetCursorPos().x - option->GetCursorRad().x) &&
		    (pos.y) <= (option->GetCursorPos().y + option->GetCursorRad().y) &&
		    (pos.y + size.y) >= (option->GetCursorPos().y - option->GetCursorRad().y) &&
		    option->GetActionLongPush(UI_SELECT)) {
			if (findUI_[i]->GetSize().x)
				isFindUI_[i] = true;
		} else {
			isFindUI_[i] = false;
		}
	}

	XINPUT_STATE joyState;

	if (inputCoolTimer_ > 0) {
		inputCoolTimer_--;
	}

	// 行動選択
	if (isSelect_) {

		if (input_->GetJoystickState(0, joyState)) {

			if ((input_->PushKey(DIK_LEFT) || option->GetActionTrigger(L_SELECT)) &&
			    inputCoolTimer_ == 0) {

				if (selectNum_ > 0) {
					selectNum_--;

					// 移動時にSEを鳴らす
					if (audio_->IsPlaying(selectSE_)) {
						audio_->StopWave(selectSE_);
					}
					audio_->PlayWave(selectSE_, false, 0.4f * option->m_seVol);
				}

				inputCoolTimer_ = kInputCoolTime;

			}

			else if (
			    (input_->PushKey(DIK_RIGHT) || option->GetActionTrigger(R_SELECT)) &&
			    inputCoolTimer_ == 0) {

				if (selectNum_ < selectCommands_.size() - 1) {
					selectNum_++;

					// 移動時にSEを鳴らす
					if (audio_->IsPlaying(selectSE_)) {
						audio_->StopWave(selectSE_);
					}
					audio_->PlayWave(selectSE_, false, 0.4f * option->m_seVol);
				}

				inputCoolTimer_ = kInputCoolTime;
			}

			if ((input_->PushKey(DIK_E) || option->GetActionTrigger(ACT)) && inputCoolTimer_ == 0 &&
			    moveCommands_.size() < kMaxCommand) {

				SetMoveCommand(selectNum_);
				PopSelectCommand(selectNum_);

				inputCoolTimer_ = kInputCoolTime;

				// 行動選択時にSEを鳴らす
				if (audio_->IsPlaying(clickSE_)) {
					audio_->StopWave(clickSE_);
				}
				audio_->PlayWave(clickSE_, false, 1.0f * option->m_seVol);

			} else if (
			    (input_->PushKey(DIK_E) || option->GetActionTrigger(ACT)) && inputCoolTimer_ == 0) {
				isSelect_ = false;
				isPlayerTurn_ = true;
				inputCoolTimer_ = kInputCoolTime;

				// 行動選択時にSEを鳴らす
				if (audio_->IsPlaying(clickSE_)) {
					audio_->StopWave(clickSE_);
				}
				audio_->PlayWave(clickSE_, false, 1.0f * option->m_seVol);
			}

			if ((input_->PushKey(DIK_Q) || option->GetActionTrigger(CANCEL)) &&
			    inputCoolTimer_ == 0 && moveCommands_.size() != 0) {
				PushSelectCommand(moveCommands_.back());
				PopBackMoveCommand();
				inputCoolTimer_ = kInputCoolTime;

				// 行動キャンセル時にSEを鳴らす
				if (audio_->IsPlaying(cancelSE_)) {
					audio_->StopWave(cancelSE_);
				}
				audio_->PlayWave(cancelSE_, false, 1.0f * option->m_seVol);
			}

			if (selectNum_ >= selectCommands_.size()) {
				selectNum_ = int(selectCommands_.size() - 1);
			}
		}

	} else {
	}

	UpdateMoveCommandsNum();
}

void Player::MoveTurn() {

	if (!isSelect_) {

		// リスト内が空でないなら行動開始
		if (moveCommands_.empty() == false) {

			if (isMove_ == false) {
				// 次の行動コマンドを現在の行動コマンドに設定
				currentMoveCommand_ = GetNextCommand();
				// 先頭の行動コマンドを削除
				PopFrontMoveCommand();
				// 数字更新
				UpdateMoveCommandsNum();
				// 行動開始フラグを立てる
				MoveTimer_ = kMoveTime / gameSpeed_->GetGameSpeed();
				isMove_ = true;
			}

		} else if (isMove_ == false) {
			SetSelectCommands();
			isPlayerTurn_ = false;
		}
	}

	// 行動フラグが立っていたら行動開始
	if (isMove_) {
		Move(currentMoveCommand_);
	} else {
	}

	worldTransform_.UpdateMatrix();
	for (int i = 0; i < 4; i++) {
		worldTransformEffect_[i].UpdateMatrix();
	}
}

void Player::Move(Command& command) {

	switch (command) {
	case MoveLeft:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			int tmpX = GetGridX() - 1;
			int tmpZ = GetGridZ();

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);
		}

		velocity_ = {-1.0f / 6.0f * gameSpeed_->GetGameSpeed(), 0.0f, 0.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveRight:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			int tmpX = GetGridX() + 1;
			int tmpZ = GetGridZ();

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);
		}

		velocity_ = {1.0f / 6.0f * gameSpeed_->GetGameSpeed(), 0.0f, 0.0f};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveUp:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			audio_->PlayWave(upMoveSE_);

			int tmpX = GetGridX();
			int tmpZ = GetGridZ() - 1;

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);
		}

		velocity_ = {0.0f, 0.0f, 1.0f / 6.0f * gameSpeed_->GetGameSpeed()};

		worldTransform_.translation_ += velocity_;

		break;
	case MoveDown:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			int tmpX = GetGridX();
			int tmpZ = GetGridZ() + 1;

			tmpX = IntClamp(tmpX, 0, 5);
			tmpZ = IntClamp(tmpZ, 0, 5);

			// 障害物があったらストップコマンドに移行
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
				command = Stop;
				break;
			}

			collisionManager_->SetCollision(tmpX, tmpZ);
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			SetGrid(tmpX, tmpZ);
		}

		velocity_ = {0.0f, 0.0f, -1.0f / 6.0f * gameSpeed_->GetGameSpeed()};

		worldTransform_.translation_ += velocity_;

		break;
	default:
	case Stop:

		velocity_ = {0.0f, 0.0f, 0.0f};

		currentTex_ = textures_[3];

		break;

	case AttackCross:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			for (int i = 0; i < 4; i++) {
				worldTransformEffect_[i].translation_ = Vector3(0.0f, 0.0f, 0.0f);
			}

			collisionManager_->SetAttackCross(GetGridX(), GetGridZ(), PlayerAttack);

			audio_->PlayWave(crossAttackSE_);
		}

		if (MoveTimer_ % 5 == 0) {
			for (int i = 0; i < 4; i++) {
				worldTransformEffect_[i].translation_.y = -3.0f;
			}
		}

		for (int i = 0; i < 4; i++) {
			velocity_ = {
			    cosf(i * 3.14f / 2.0f) / 3.0f * gameSpeed_->GetGameSpeed(), 1.0f,
			    sinf(i * 3.14f / 2.0f) / 3.0f * gameSpeed_->GetGameSpeed()};
			worldTransformEffect_[i].translation_ += velocity_;
		}

		currentTex_ = textures_[0];

		break;
	case AttackCircle:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), PlayerAttack);
		}

		velocity_ = {0.0f, 0.0f, 0.0f};

		currentTex_ = textures_[1];

		break;
	case Guard:

		velocity_ = {0.0f, 0.0f, 0.0f};

		isGuard_ = true;

		currentTex_ = textures_[2];

		break;
	}

	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -25.0f, 25.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -25.0f, 25.0f);

	if (--MoveTimer_ <= 0) {
		velocity_.y = 0.0f;
		currentTex_ = textures_[0];
		currentMoveCommand_ = Stop;
		collisionManager_->ResetAttack();
		isMove_ = false;
		isHit_ = false;
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

		// 選択中のコマンドの色を変更する
		if (i == selectNum_) {
			selectCommandNumSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, 0.5f});
		} else {
			selectCommandNumSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		}
		selectCommandNumSprite_[i]->Draw();
	}

	for (int i = 0; i < selectCommands_.size(); i++) {
		if (isFindUI_[i] == true) {
			findUI_[i]->Draw();
		}
	}

	int divideNum = 100;
	int lifeIndex = life_;
	for (int i = 0; i < 3; i++) {

		lifeNum_[i] = lifeIndex / divideNum;
		
		lifeIndex = lifeIndex % divideNum;
		divideNum = divideNum / 10;
		
		hpSprite_[i]->SetTextureRect(
		    {
		        lifeNum_[i] * 1024.0f,
		        0.0f,
		    },
		    {1024.0f, 1024.0f});

		hpSprite_[i]->Draw();
	}

	// currentNumSprite_->Draw();
}

void Player::UpdateMoveCommandsNum() {

	for (int i = 0; i < moveCommands_.size(); i++) {

		auto itr = moveCommands_.begin();

		for (int k = 0; k < i; k++) {
			itr++;
		}

		int num = *itr;

		commandNumSprite_[i]->SetTextureRect({num * 1024.0f, 0.0f}, {1024.0f, 1024.0f});
	}

	for (int i = 0; i < selectCommands_.size(); i++) {

		auto itr = selectCommands_.begin();

		for (int k = 0; k < i; k++) {
			itr++;
		}

		int num = *itr;

		selectCommandNumSprite_[i]->SetTextureRect({num * 1024.0f, 0.0f}, {1024.0f, 1024.0f});
	}

	currentNumSprite_->SetPosition({74.0f, selectNum_ * 64.0f + 246.0f});
}
