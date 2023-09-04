#include "Enemy.h"

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

void Enemy::Initialize(Sprite* sprite) {

	input_ = Input::GetInstance();

	enemySprite_ = sprite;
	position_ = {640.0f, 500.0f, 0.0f};
	velocity_ = {0.0f, 0.0f, 0.0f};
	SetSpritePosition();
}

void Enemy::Update() {

#ifdef _DEBUG

	ImGui::Begin("EnemyState");
	ImGui::Text("current Command %d", currentMoveCommand_);
	ImGui::Text("move coolTime %d", coolTime_);
	ImGui::End();

#endif // _DEBUG

	// リストの要素が空なら新たに設定する
	if (GetmoveCommands().empty()) {
		SetMoveCommand(3);
	} else {

		// リスト内が空でないなら行動開始
		if (GetmoveCommands().empty() == false) {
			if (isMove_ == false && isCoolTime == false) {

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

	// 行動フラグが立っていたら行動開始
	if (isMove_) {
		Move(currentMoveCommand_);
	} else {
	}
}

void Enemy::Move(Command command) {

	switch (command) {
	default:
	case Stop:

		velocity_ = {0.0f, 0.0f, 0.0f};

		// 緑に設定
		enemySprite_->SetColor({0.0f, 1.0f, 0.0f, 1.0f});

		break;
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

		// 赤に設定
		enemySprite_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});

		break;
	case Guard:

		// 青に設定
		enemySprite_->SetColor({0.0f, 0.0f, 1.0f, 1.0f});

		break;
	}

	position_.x = Clamp(position_.x, 32.0f, 1248.0f);
	position_.y = Clamp(position_.y, 0.0f, 500.0f);
	SetSpritePosition();

	if (--MoveTimer_ <= 0) {
		velocity_ = {0.0f, 0.0f, 0.0f};
		enemySprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		isCoolTime = true;
	}
	if (isCoolTime) {
		if (--coolTime_ <= 0) {
			coolTime_ = kEnemyCoolTime_;
			isCoolTime = false;
			isMove_ = false;
		}
	}
}

void Enemy::Draw() { enemySprite_->Draw(); }
