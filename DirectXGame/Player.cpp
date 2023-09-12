#include "Player.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

void Player::Initialize(
    const std::vector<Model*>& models, const std::vector<uint32_t>& textures,
    const std::vector<uint32_t>& sounds) {

	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();
	gameSpeed_ = GameSpeed::GetInstance();

	SetModels(models);
	SetTextures(textures);
	SetSounds(sounds);

	currentTex_ = textures_[0];

	findUITexture_ = TextureManager::Load("findUI.png");

	hpFrameSprite_.reset(Sprite::Create(textures_[2], {-100.0f, 0.0f}));
	hpFrameSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1024.0f, 1024.0f});

	nextUISprite_.reset(Sprite::Create(textures_[8], {0.0f, 0.0f}));
	nextUISprite_->SetSize({128.0f, 64.0f});
	nextUISprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1024.0f, 256.0f});
	nextUISprite_->SetPosition({10.0f, 10.0f});

	for (int i = 0; i < kMaxCommand; i++) {
		commandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		commandNumSprite_[i]->SetSize({64.0f, 64.0f});
		commandNumSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {1024.0f, 1024.0f});
		commandNumSprite_[i]->SetPosition({(kMaxCommand - 1 + i) * 64.0f + 10.0f, 10.0f});
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
		selectCommandNumSprite_[i]->SetPosition({10.0f, i * 64.0f + 158.0f});
	}

	for (int i = 0; i < kMaxSelectNum; i++) {
		findUI_[i].reset(Sprite::Create(findUITexture_, {0.0f, 0.0f}));
		findUI_[i]->SetSize({64.0f, 64.0f});
		findUI_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {32.0f, 32.0f});
		findUI_[i]->SetPosition(
		    {selectCommandNumSprite_[i]->GetPosition().x,
		     selectCommandNumSprite_[i]->GetPosition().y});
	}

	for (int i = 0; i < 3; i++) {
		hpSprite_[i].reset(Sprite::Create(textures_[6], {0.0f, 0.0f}));
		hpSprite_[i]->SetSize({64.0f, 64.0f});
		hpSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {1024.0f, 1024.0f});
		hpSprite_[i]->SetPosition({(float)(i + 1) * 64, 10.0f});
	}

	currentNumSprite_.reset(Sprite::Create(textures_[0], {0.0f, 0.0f}));
	currentNumSprite_->SetSize({16.0f, 16.0f});
	currentNumSprite_->SetPosition({selectNum_ * 64.0f + 316.0f, 550.0f});

	velocity_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Initialize();
	worldTransform_.translation_ = Vector3(-25.0f, 2.0f, 5.0f);
	worldTransform_.UpdateMatrix();

	for (int i = 0; i < 8; i++) {
		worldTransformEffect_[i].Initialize();
		worldTransformEffect_[i].scale_ *= 2.0f;
		worldTransformEffect_[i].parent_ = &worldTransform_;
		worldTransformEffect_[i].UpdateMatrix();
	}

	moveAngle_ = 0.0f;

	SetSelectCommands();
	SetGrid(0, 2);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

	crossAttackSE_ = audio_->LoadWave("SE/crossattack.wav");
	upMoveSE_ = audio_->LoadWave("SE/up.wav");
	clickSE_ = audio_->LoadWave("SE/click.wav");
	selectSE_ = audio_->LoadWave("SE/select.wav");
	cancelSE_ = audio_->LoadWave("SE/cancel.wav");
	fallSE_ = audio_->LoadWave("SE/fall.wav");
}

void Player::Update(const ViewProjection& viewProjection, Option* option) {

#ifdef _DEBUG

	ImGui::Begin("State");
	ImGui::Text("HP %d", life_);
	ImGui::Text("special Count %d", specialCount_);
	ImGui::End();

	if (input_->TriggerKey(DIK_0)) {
		life_ -= 20;
	}

#endif // _DEBUG

	if (isStart_ == false) {

		if (startCount_ <= 0) {
			worldTransform_.translation_.y = 2.0f;
			velocity_ = {0.0f, 0.0f, 0.0f};
			isStart_ = true;
		}

		if (startCount_ == 60) {
			velocity_ = {0.0f, -2.0f, 0.0f};
		}

		velocity_ -= {0.0f, 0.2f, 0.0f};

		worldTransform_.translation_ += velocity_;

		if (worldTransform_.translation_.y <= 2.0f) {
			worldTransform_.translation_.y = 2.0f;
			velocity_ = {0.0f, 3.0f * float(startCount_ / 60.0f), 0.0f};

			if (startCount_ >= 10) {
				audio_->PlayWave(fallSE_);
			}
		}

		startCount_--;

		worldTransform_.UpdateMatrix();

	} else {

		if (isHit_ == false) {

			if (collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), EnemyAttack)) {

				// ガード状態なら半減し、その後ガードを解除する
				if (isGuard_) {
					life_ -= 5;
					isGuard_ = false;
				} else {
					life_ -= 10;
				}

				audio_->PlayWave(sounds_[0], false, option->m_seVol);
				isHit_ = true;

			} else if (collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), EnemySpecialAttack)) {

				// ガード状態なら半減し、その後ガードを解除する
				if (isGuard_) {
					life_ -= 10;
					isGuard_ = false;
				} else {
					life_ -= 20;
				}

				audio_->PlayWave(sounds_[0], false, option->m_seVol);
				isHit_ = true;
			}

		}
		// アタックが終了したらヒットフラグを降ろす
		else if (isHit_ == true && collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), 0)) {

			isHit_ = false;
		}

		// 体力が0以下で死亡
		if (life_ <= 0) {
			life_ = 0;
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

		// XINPUT_STATE joyState;

		if (inputCoolTimer_ > 0) {
			inputCoolTimer_--;
		}

		// 行動選択
		if (isSelect_) {

			if ((input_->TriggerKey(DIK_UP) || option->GetActionTrigger(U_SELECT)) &&
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
			    (input_->TriggerKey(DIK_DOWN) || option->GetActionTrigger(D_SELECT)) &&
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

			// スペシャルカウントが溜まっていたら特殊攻撃可能にする
			if (specialCount_ >= kMaxSpecialCount && option->GetActionTrigger(SPECIAL)) {

				// 攻撃、ガードコマンドリセット
				for (int i = 0; i < kMaxCommand; i++) {
					selectCommands_.pop_back();
				}

				for (int i = 0; i < kMaxCommand; i++) {

					selectCommands_.push_back(S_PlayerAttack);
				}

				specialCount_ = 0;

			} else if (
			    (input_->TriggerKey(DIK_RETURN) || option->GetActionTrigger(ACT)) &&
			    inputCoolTimer_ == 0 && moveCommands_.size() < kMaxCommand) {

				SetMoveCommand(selectNum_);
				PopSelectCommand(selectNum_);

				inputCoolTimer_ = kInputCoolTime;

				// 行動選択時にSEを鳴らす
				if (audio_->IsPlaying(clickSE_)) {
					audio_->StopWave(clickSE_);
				}
				audio_->PlayWave(clickSE_, false, 1.0f * option->m_seVol);

			} else if (
			    (input_->TriggerKey(DIK_RETURN) || option->GetActionTrigger(ACT)) &&
			    inputCoolTimer_ == 0) {
				isSelect_ = false;
				isPlayerTurn_ = true;
				inputCoolTimer_ = kInputCoolTime;

				// 行動選択時にSEを鳴らす
				if (audio_->IsPlaying(clickSE_)) {
					audio_->StopWave(clickSE_);
				}
				audio_->PlayWave(clickSE_, false, 1.0f * option->m_seVol);
			}

			if ((input_->TriggerKey(DIK_BACKSPACE) || option->GetActionTrigger(CANCEL)) &&
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

		} else {
		}
	}

	UpdateMoveCommandsNum();

	SetCommandSprite(viewProjection);
}

void Player::MoveTurn() {

	if (!isSelect_) {

		// 行動後に間を置く
		if (interval_ > 0) {
			interval_--;
		}
		// リスト内が空でないなら行動開始
		else if (moveCommands_.empty() == false) {

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
	for (int i = 0; i < 8; i++) {
		worldTransformEffect_[i].UpdateMatrix();
	}
}

void Player::Move(Command& command) {

	switch (command) {
	case MoveLeft:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			if (specialCount_ < kMaxSpecialCount) {
				specialCount_++;
			}

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

			if (specialCount_ < kMaxSpecialCount) {
				specialCount_++;
			}

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

			if (specialCount_ < kMaxSpecialCount) {
				specialCount_++;
			}

			/*audio_->PlayWave(upMoveSE_);*/

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

			if (specialCount_ < kMaxSpecialCount) {
				specialCount_++;
			}

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

			for (int i = 0; i < 8; i++) {
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

			for (int i = 0; i < 8; i++) {
				worldTransformEffect_[i].translation_ = Vector3(0.0f, 0.0f, 0.0f);
			}

			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), PlayerAttack);
		}

		if (MoveTimer_ % 5 == 0) {
			for (int i = 0; i < 8; i++) {
				worldTransformEffect_[i].translation_.y = -3.0f;
			}
		}

		for (int i = 0; i < 8; i++) {
			if (i == 2 || i == 6 || i == 7) {
				velocity_.x = cosf(2.0f * 3.14f / 2.0f) / 5.5f * gameSpeed_->GetGameSpeed();
			} else if (i == 0 || i == 4 || i == 5) {
				velocity_.x = cosf(0.0f * 3.14f / 2.0f) / 5.5f * gameSpeed_->GetGameSpeed();
			} else {
				velocity_.x = cosf(i * 3.14f / 2.0f) / 5.5f * gameSpeed_->GetGameSpeed();
			}

			velocity_.y = 1.0f;

			if (i == 1 || i == 5 || i == 6) {
				velocity_.z = sinf(1.0f * 3.14f / 2.0f) / 5.5f * gameSpeed_->GetGameSpeed();
			} else if (i == 3 || i == 4 || i == 7) {
				velocity_.z = sinf(3.0f * 3.14f / 2.0f) / 5.5f * gameSpeed_->GetGameSpeed();
			} else {
				velocity_.z = sinf(i * 3.14f / 2.0f) / 5.5f * gameSpeed_->GetGameSpeed();
			}

			worldTransformEffect_[i].translation_ += velocity_;
		}

		currentTex_ = textures_[1];

		break;
	case Guard:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			if (specialCount_ < kMaxSpecialCount) {
				specialCount_++;
			}

			isGuard_ = true;

			for (int i = 0; i < 8; i++) {
				worldTransformEffect_[i].translation_ = {
				    cosf((i * 10.0f) * 3.14f / 180.0f), 0.0f, sinf((i * 10.0f) * 3.14f / 180.0f)};
			}
		}

		if (MoveTimer_ % 5 == 0) {
			for (int i = 0; i < 8; i++) {
				worldTransformEffect_[i].translation_.y = -3.0f;
			}
		}

		for (int i = 0; i < 5; i++) {
			const float length = 4.0f;
			float radius = moveAngle_ * 3.14f / 180.0f;

			velocity_.x = cosf(i * 10.0f + radius) * length;
			velocity_.y = 1.0f;
			velocity_.z = sinf(i * 10.0f + radius) * length;

			worldTransformEffect_[i].translation_.x = velocity_.x;
			worldTransformEffect_[i].translation_.y += velocity_.y;
			worldTransformEffect_[i].translation_.z = velocity_.z;

			moveAngle_ += 1.0f;
		}

		currentTex_ = textures_[2];

		break;
	case S_PlayerAttack:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {
			collisionManager_->SetAttackCross(GetGridX(), GetGridZ(), PlayerSpecialAttack);
			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), PlayerSpecialAttack);
		}

		break;
	}

	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -25.0f, 25.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -25.0f, 25.0f);

	if (--MoveTimer_ <= 0) {
		moveAngle_ = 0.0f;
		velocity_ = {0.0f, 0.0f, 0.0f};

		if (isGuard_) {
			currentTex_ = textures_[9];
		} else {
			currentTex_ = textures_[0];
		}

		currentMoveCommand_ = Stop;
		collisionManager_->ResetAttack();
		interval_ = kMaxInterval;
		isMove_ = false;
		isHit_ = false;
	}
}

void Player::Draw(const ViewProjection& viewProjection) {

	models_[0]->Draw(worldTransform_, viewProjection, currentTex_);

	/*if (currentMoveCommand_ == AttackCross || currentMoveCommand_ == AttackCircle) {

	    for (int i = 0; i < 8; i++) {
	        models_[1]->Draw(worldTransformEffect_[i], viewProjection, textures_[5]);
	    }
	}*/

	if (currentMoveCommand_ == Guard) {

		for (int i = 0; i < 5; i++) {
			models_[2]->Draw(worldTransformEffect_[i], viewProjection, textures_[9]);
		}
	}
}

void Player::DrawUI() {

	for (int i = 0; i < GetmoveCommands().size(); i++) {
		commandNumSprite_[i]->Draw();
	}

	for (int i = 0; i < selectCommands_.size(); i++) {

		// 選択中のコマンドの色を変更する
		if (i == selectNum_ && moveCommands_.size() < kMaxCommand && isSelect_) {
			selectCommandNumSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
			selectCommandNumSprite_[i]->SetPosition({20.0f, i * 64.0f + 158.0f});
		} else {
			selectCommandNumSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, 0.3f});
			selectCommandNumSprite_[i]->SetPosition({10.0f, i * 64.0f + 158.0f});
		}
		selectCommandNumSprite_[i]->Draw();
	}

	for (int i = 0; i < selectCommands_.size(); i++) {
		if (isFindUI_[i] == true) {
			findUI_[i]->Draw();
		}
	}

	/*int divideNum = 100;
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
	}*/

	hpFrameSprite_->Draw();
	nextUISprite_->Draw();

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

void Player::SetCommandSprite(const ViewProjection& viewProjection) {

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle = Vector3(
		    worldTransform_.matWorld_.m[3][0] - 2.0f, worldTransform_.matWorld_.m[3][1] + 15.0f,
		    worldTransform_.matWorld_.m[3][2]);

		// ビューポート行列
		Matrix4x4 matViewport =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport =
		    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

		// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
		positionReticle = Transform(positionReticle, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		hpFrameSprite_->SetSize({(128.0f * (float(life_) / kMaxLife)), 16.0f});
		hpFrameSprite_->SetPosition(Vector2(positionReticle.x - 16.0f, positionReticle.y));
	}
}

void Player::Reset() {

	isHit_ = false;
	isDead_ = false;
	isStart_ = false;
	startCount_ = 60;
	life_ = kMaxLife;

	moveAngle_ = 0.0f;

	MoveTimer_ = 0;
	isSelect_ = true;
	selectNum_ = 0;
	isMove_ = false;
	isAttack_ = false;
	isGuard_ = false;
	isPlayerTurn_ = false;
	currentMoveCommand_ = Stop;

	inputCoolTimer_ = 0;

	specialCount_ = 0;
	interval_ = 0;

	velocity_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = Vector3(-25.0f, 2.0f, 5.0f);
	worldTransform_.UpdateMatrix();

	SetSelectCommands();
	SetGrid(0, 2);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());
}
