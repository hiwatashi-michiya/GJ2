#include "Enemy.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

void Enemy::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {

	input_ = Input::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();

	SetModels(models);
	SetTextures(textures);

	currentTex_ = textures_[0];

	for (int i = 0; i < kMaxEnemyCommand; i++) {
		commandNumSprite_[i].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
		commandNumSprite_[i]->SetSize({64.0f, 64.0f});
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
	SetSelectCommands(kMaxEnemySelectNum);
	SetGrid(5, 3);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

}

void Enemy::Update() {

#ifdef _DEBUG

	ImGui::Begin("EnemyState");
	ImGui::Text("HP %d", life_);
	ImGui::End();

	if (input_->TriggerKey(DIK_2)) {
		life_ -= 20;
	}

#endif // _DEBUG

	if (isHit_ == false) {

		//プレイヤーの攻撃を受けたら
		if (collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), PlayerAttack)) {

			life_ -= 10;
			isHit_ = true;

		}

	}
	//アタックが終了したらヒットフラグを降ろす
	else if (isHit_ == true && collisionManager_->IsHitAttack(GetGridX(), GetGridZ(), 0)) {

		isHit_ = false;

	}

	//体力が0以下で死亡
	if (life_ <= 0) {
		isDead_ = true;
	}

	/*XINPUT_STATE joyState;*/

	if (inputCoolTimer_ > 0) {
		inputCoolTimer_--;
	}

	// リストの要素が空なら新たに設定する
	if (isSelect_) {

		if (moveCommands_.size() < kMaxEnemyCommand) {

				// ランダム行動
			selectNum_ = rand() % selectCommands_.size();

			SetMoveCommand(selectNum_);
			PopSelectCommand(selectNum_);

		}
		else if(player_->GetIsSelect() == false){

			isSelect_ = false;
			isEnemyTurn_ = true;
			inputCoolTimer_ = kInputCoolTime;

		}

		UpdateMoveCommandsNum();
	} else {

	}

	worldTransform_.UpdateMatrix();
}

void Enemy::MoveTurn() {

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
				MoveTimer_ = kMoveTime;
				isMove_ = true;
			}
		} 
		else if(isMove_ == false) {
			SetSelectCommands(kMaxEnemySelectNum);
			isEnemyTurn_ = false;
		}

	}

	// 行動フラグが立っていたら行動開始
	if (isMove_) {
		Move(currentMoveCommand_);
	} else {
	}

	worldTransform_.UpdateMatrix();

}

void Enemy::Move(Command& command) {

	switch (command) {
	case MoveLeft:

		if (MoveTimer_ == 60) {

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
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
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
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
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
			if (collisionManager_->IsHit(tmpX, tmpZ, 1)) {
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

			collisionManager_->SetAttackCross(GetGridX(), GetGridZ(), EnemyAttack);

		}

		currentTex_ = textures_[0];

		break;
	case AttackCircle:

		if (MoveTimer_ == 60) {

			collisionManager_->SetAttackCircle(GetGridX(), GetGridZ(), EnemyAttack);

		}

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
		collisionManager_->ResetAttack();
		isMove_ = false;
	}
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	models_[0]->Draw(worldTransform_, viewProjection, currentTex_);
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

void Enemy::SetPosition(int x, int z) {

	x = IntClamp(x, 0, 5);
	z = IntClamp(z, 0, 5);


	worldTransform_.translation_ = Vector3(-25.0f + x * 10.0f, 2.0f, 25.0f - z * 10.0f);
	worldTransform_.UpdateMatrix();
	SetGrid(x, z);
	collisionManager_->SetCollision(GetGridX(), GetGridZ());

}
