#include "Enemy.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

void Enemy::Initialize(
    const std::vector<Model*>& models, const std::vector<uint32_t>& textures,
    const std::vector<uint32_t>& sounds) {

	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();
	gameSpeed_ = GameSpeed::GetInstance();

	SetModels(models);
	SetTextures(textures);
	SetSounds(sounds);

	guardModel_.reset(Model::CreateFromOBJ("guard", true));

	guardTex_ = TextureManager::Load("guard/guard.png");

	currentTex_ = textures_[0];

	for (int i = 0; i < kMaxEnemyCommand; i++) {
		commandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		commandNumSprite_[i]->SetSize({32.0f, 32.0f});
		commandNumSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {64.0f, 64.0f});
		commandNumSprite_[i]->SetPosition({1200.0f, (kMaxEnemyCommand - 1 - i) * 64.0f + 10.0f});
	}

	for (int i = 0; i < kMaxEnemySelectNum; i++) {

		selectCommandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		selectCommandNumSprite_[i]->SetSize({64.0f, 64.0f});
		selectCommandNumSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {64.0f, 64.0f});
		selectCommandNumSprite_[i]->SetPosition({i * 64.0f + 600.0f, 120.0f});
	}

	currentNumSprite_.reset(Sprite::Create(textures_[0], {0.0f, 0.0f}));
	currentNumSprite_->SetSize({16.0f, 16.0f});
	currentNumSprite_->SetPosition({selectNum_ * 64.0f + 610.0f, 80.0f});

	velocity_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Initialize();
	worldTransform_.translation_ = Vector3(25.0f, 2.0f, -5.0f);
	worldTransform_.UpdateMatrix();

	worldTransformGuard_.Initialize();
	worldTransformGuard_.parent_ = &worldTransform_;
	worldTransformGuard_.translation_.y = 3.0f;
	worldTransformGuard_.scale_ *= 5.0f;
	worldTransformGuard_.UpdateMatrix();

	SetSelectCommands();
	SetGrid(0, 0);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

	movePattern_ = MovePattern(rand() % 3);

	effect_.Initialize();
	effect_.SetEffectType(Crash);

}

void Enemy::Update(const ViewProjection& viewProjection, Option* option) {

#ifdef _DEBUG

	ImGui::Begin("EnemyState");
	ImGui::Text("HP %d", life_);
	ImGui::Text("count %d", rushCount_);
	ImGui::End();

	if (input_->TriggerKey(DIK_9)) {
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
		}

		startCount_--;

		worldTransform_.UpdateMatrix();
	}
	else if (isEffect_ == false) {

		if (isHit_ == false) {

			// プレイヤーの攻撃を受けたら
			if (collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), PlayerAttack)) {

				// ガード状態なら半減し、その後ガードを解除する
				if (isGuard_) {
					life_ -= 5;
					isGuard_ = false;
				} else {
					life_ -= 10;
				}
				audio_->PlayWave(sounds_[0], false, option->m_seVol);

				isHit_ = true;

			} else if (collisionManager_->IsHitAttack(
			               GetGridX(), GetGridZ(), PlayerSpecialAttack)) {

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

		// 体力半分以下で行動変化
		if (life_ <= (maxLife_ / 2) && isFormChange_ == false) {
			// 半分以下で確定行動
			movePattern_ = E_Special;
			isFormChange_ = true;
		}

		// 体力が0以下で死亡。エフェクト開始
		if (life_ <= 0) {
			collisionManager_->RemoveCollision(GetGridX(), GetGridZ());
			effect_.Reset(60 / gameSpeed_->GetGameSpeed());
			effect_.SetStartPosition(worldTransform_.translation_);
			effect_.SetEffect();
			isEffect_ = true;
		}

		/*XINPUT_STATE joyState;*/

		if (inputCoolTimer_ > 0) {
			inputCoolTimer_--;
		}

		// リストの要素が空なら新たに設定する
		if (isSelect_) {

			SetEnemyMovePattern();

			UpdateMoveCommandsNum();

		} else {
		}

		worldTransform_.UpdateMatrix();

		SetCommandSprite(viewProjection);

	}
	else {
		effect_.Update();

		if (effect_.IsDead()) {
			isDead_ = true;
		}

	}

	worldTransform_.UpdateMatrix();
	worldTransformGuard_.UpdateMatrix();

}

void Enemy::MoveTurn(const ViewProjection& viewProjection) {

	if (!isSelect_) {

		//行動後に間を置く
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
		} 
		else if(isMove_ == false) {
			SetSelectCommands();
			isEnemyTurn_ = false;
		}

	}

	// 行動フラグが立っていたら行動開始
	if (isMove_) {
		Move(currentMoveCommand_);
	} else {
	}

	worldTransform_.UpdateMatrix();
	worldTransformGuard_.UpdateMatrix();

	SetCommandSprite(viewProjection);

}

void Enemy::Move(Command& command) {

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

			collisionManager_->SetAttackCross(GetGridX(), GetGridZ(), EnemyAttack);

		}

		currentTex_ = textures_[0];

		break;
	case AttackCircle:

		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed()) {

			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), EnemyAttack);

		}

		velocity_ = {0.0f, 0.0f, 0.0f};

		currentTex_ = textures_[1];

		break;
	case Guard:

		velocity_ = {0.0f, 0.0f, 0.0f};

		isGuard_ = true;

		currentTex_ = textures_[2];

		break;
	case S_EnemyAttack:

		//一回目
		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed() && moveCommands_.size() == 2) {

			collisionManager_->SetAttackCross(GetGridX(), GetGridZ(), EnemyAttack);
		}

		//二回目
		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed() && moveCommands_.size() == 1) {

			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), EnemyAttack);
		}

		//三回目
		if (MoveTimer_ == kMoveTime / gameSpeed_->GetGameSpeed() && moveCommands_.size() == 0) {

			collisionManager_->SetAttackCross(GetGridX(), GetGridZ(), EnemySpecialAttack);
			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), EnemySpecialAttack);

		}

		break;

	}

	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -25.0f, 25.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -25.0f, 25.0f);

	if (--MoveTimer_ <= 0) {
		velocity_.y = 0.0f;
		currentTex_ = textures_[0];
		collisionManager_->ResetAttack();
		interval_ = kMaxInterval;
		isMove_ = false;
	}
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	if (isEffect_) {
		effect_.Draw(viewProjection);
	}
	else {
		models_[0]->Draw(worldTransform_, viewProjection, currentTex_);

		if (isGuard_) {
			guardModel_->Draw(worldTransformGuard_, viewProjection, guardTex_);
		}

	}
}

void Enemy::DrawUI() {

	for (int i = 0; i < GetmoveCommands().size(); i++) {
		commandNumSprite_[i]->Draw();
	}

	/*for (int i = 0; i < selectCommands_.size(); i++) {
		selectCommandNumSprite_[i]->Draw();
	}

	currentNumSprite_->Draw();*/
}

void Enemy::UpdateMoveCommandsNum() {

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

	currentNumSprite_->SetPosition({selectNum_ * 64.0f + 610.0f, 80.0f});
}

void Enemy::SetGridPosition(int x, int z) {

	collisionManager_->RemoveCollision(GetGridX(), GetGridZ());

	x = IntClamp(x, 0, 5);
	z = IntClamp(z, 0, 5);


	worldTransform_.translation_ = Vector3(-25.0f + x * 10.0f, 2.0f, 25.0f - z * 10.0f);
	worldTransform_.UpdateMatrix();
	SetGrid(x, z);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

}

void Enemy::SetCommandSprite(const ViewProjection& viewProjection) {

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle = Vector3(
		    worldTransform_.matWorld_.m[3][0] - 2.0f,
			worldTransform_.matWorld_.m[3][1] + 15.0f,
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
		for (int i = 0; i < GetmoveCommands().size(); i++) {

			commandNumSprite_[i]->SetPosition(Vector2(positionReticle.x + i * 32.0f, positionReticle.y));

		}
		
	}

}

void Enemy::SetEnemyMovePattern() {

	//コマンド決定
	if (moveCommands_.size() < kMaxEnemyCommand) {

		Command command;

		int32_t randNum = 0;

		switch (movePattern_) {
		default:
		case E_Move:

			// 左右移動決定
			if (GetGridX() == 5) {

				command = Command(MoveLeft);
				moveCommands_.push_back(command);

			}
			else if (GetGridX() == 0) {
				command = Command(MoveRight);
				moveCommands_.push_back(command);
			}
			else {
				command = Command(rand() % 2);
				moveCommands_.push_back(command);
			}

			// 上下移動決定
			if (GetGridZ() == 5) {
				command = Command(MoveUp);
				moveCommands_.push_back(command);
			}
			else if (GetGridZ() == 0) {
				command = Command(MoveDown);
				moveCommands_.push_back(command);
			}
			else {
				command = Command(rand() % 2 + 2);
				moveCommands_.push_back(command);
			}

			// 残りの移動決定
			command = Command(rand() % 5);
			moveCommands_.push_back(command);

			// 次の行動パターンを決定
			randNum = rand() % 2;

			switch (randNum) {
			default:
			case 0:

				movePattern_ = E_Attack;

				break;
			case 1:

				movePattern_ = E_Guard;

				break;
			}

			//ラッシュカウント加算
			rushCount_++;

			break;
		case E_Attack:

			//移動決定
			command = Command(rand() % 5);
			moveCommands_.push_back(command);

			//攻撃決定
			command = Command(rand() % 2 + 5);
			moveCommands_.push_back(command);

			//残りのコマンド決定
			command = Command(rand() % 7);
			moveCommands_.push_back(command);

			// 次の行動パターンを決定
			randNum = rand() % 2;

			switch (randNum) {
			default:
			case 0:

				movePattern_ = E_Move;

				break;
			case 1:

				movePattern_ = E_Guard;

				break;
			}

			break;
		case E_Guard:

			//移動決定
			command = Command(rand() % 5);
			moveCommands_.push_back(command);

			//移動決定
			command = Command(rand() % 5);
			moveCommands_.push_back(command);

			//移動後にガード
			command = Guard;
			moveCommands_.push_back(command);

			// 次の行動パターンを決定
			randNum = rand() % 2;

			switch (randNum) {
			default:
			case 0:

				movePattern_ = E_Move;

				break;
			case 1:

				movePattern_ = E_Attack;

				break;
			}

			// ラッシュカウント加算
			rushCount_++;

			break;
		case E_Rush:

			//攻撃を繰り返し決定
			command = Command(rand() % 2 + 5);
			moveCommands_.push_back(command);

			command = Command(rand() % 2 + 5);
			moveCommands_.push_back(command);

			command = Command(rand() % 2 + 5);
			moveCommands_.push_back(command);

			//攻撃後スタン状態に入る
			movePattern_ = E_Stan;

			break;
		case E_Stan:

			//移動停止
			command = Stop;

			for (int i = 0; i < kMaxEnemyCommand; i++) {
				moveCommands_.push_back(command);
			}

			// 次の行動パターンを決定
			randNum = rand() % 3;

			switch (randNum) {
			default:
			case 0:

				movePattern_ = E_Move;

				break;
			case 1:

				movePattern_ = E_Attack;

				break;
			case 2:

				movePattern_ = E_Guard;

				break;
			}

			break;
		case E_Special:

			//特殊攻撃をセット
			command = S_EnemyAttack;

			for (int i = 0; i < kMaxEnemyCommand; i++) {
				moveCommands_.push_back(command);
			}

			// 攻撃後スタン状態に入る
			movePattern_ = E_Stan;

			break;
		}

	}

	

	if (player_->GetIsSelect() == false) {

		//ラッシュカウントが溜まったらコマンドをラッシュに変更
		if (rushCount_ == 3 && isFormChange_ == false) {
			movePattern_ = E_Rush;
			rushCount_ = 0;
		}
		//体力半分以下でラッシュカウントが溜まったらスペシャル攻撃
		else if (rushCount_ == 5 && isFormChange_ == true) {
			movePattern_ = E_Special;
			rushCount_ = 0;
		}

		isSelect_ = false;
		isEnemyTurn_ = true;
		inputCoolTimer_ = kInputCoolTime;
	}

}

void Enemy::Reset() {

}
