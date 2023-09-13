#include "Effect.h"
#include "Player.h"
#include "Rand.h"

void Effect::Initialize() {

	effectModel_.reset(Model::CreateFromOBJ("particlecube", true));
	pillarModel_.reset(Model::CreateFromOBJ("attack", true));

	for (int i = 0; i < kMaxParticles; i++) {
		worldTransforms_[i].Initialize();
		worldTransforms_[i].scale_ *= 0.5f;
	}

	worldTransformPillar_.Initialize();
	worldTransformPillar_.scale_ *= 5.0f;

	effectTex_[0] = TextureManager::Load("particlecube/effect.png");
	effectTex_[1] = TextureManager::Load("particlecube/effect_black.png");
	effectTex_[2] = TextureManager::Load("particlecube/dust.png");
	effectTex_[3] = TextureManager::Load("attack/playerattack.png");
	dustTex_ = TextureManager::Load("particlecube/dust.png");

	player_ = std::make_unique<Player>();
}

void Effect::Update() {

	if (isDead_ == false) {

		if (--lifeTimer_ <= 0) {
			isDead_ = true;
		}

		switch (effectType_) {
		default:
		case Up:

			for (int i = 0; i < kMaxParticles; i++) {
				velocities_[i].y -= 0.3f;
				worldTransforms_[i].translation_ += velocities_[i];

				if (worldTransforms_[i].translation_.y <= 0.0f) {
					velocities_[i].y += 1.5f;
				}

				worldTransforms_[i].rotation_ += Vector3(0.3f, 0.0f, 0.0f);
				worldTransforms_[i].UpdateMatrix();
			}

			if (lifeTimer_ <= 20) {

				worldTransformPillar_.scale_ -= {0.25f, 0.25f, 0.25f};
			}

			worldTransformPillar_.UpdateMatrix();

			break;

		case Crash:

			for (int i = 0; i < kMaxParticles; i++) {

				velocities_[i].y -= 0.3f;

				worldTransforms_[i].scale_ -= Vector3(0.03f, 0.03f, 0.03f);
				worldTransforms_[i].translation_ += velocities_[i];

				if (worldTransforms_[i].translation_.y <= 0.0f) {
					velocities_[i].y = 1.0f * float(lifeTimer_ / 10);
				}

				worldTransforms_[i].UpdateMatrix();
			}

			break;

		case Dust:

			for (int i = 0; i < kMaxParticles; i++) {

				worldTransforms_[i].translation_.x += velocities_[i].x;
				worldTransforms_[i].translation_.y = velocities_[i].y;
				worldTransforms_[i].translation_.z += velocities_[i].z;

				worldTransforms_[i].rotation_ += Vector3(0.3f, 0.3f, 0.3f);

				switch (currentMoveCommand_) {
				case MoveUp:
					velocities_[i].z += 0.01f;

					break;
				case MoveDown:
					velocities_[i].z -= 0.01f;

					break;
				case MoveLeft:
					velocities_[i].x -= 0.01f;

					break;
				case MoveRight:
					velocities_[i].x += 0.01f;

					break;

				default:
				case Stop:
					break;

				case AttackCross:
					break;

				case AttackCircle:
					break;

				case Guard:
					break;

				case S_PlayerAttack:
					break;

				case S_EnemyAttack:
					break;
				}

				worldTransforms_[i].UpdateMatrix();
			}

			break;
		}
	}
}

void Effect::Draw(const ViewProjection& viewProjection) {

	if (isDead_ == false) {

		for (int i = 0; i < kMaxParticles; i++) {
			if (effectType_ == Dust) {
				effectModel_->Draw(worldTransforms_[i], viewProjection, dustTex_);
			} else {
				effectModel_->Draw(worldTransforms_[i], viewProjection, effectTex_[i % 2]);
			}
		}

		if (effectType_ == Up) {
			pillarModel_->Draw(worldTransformPillar_, viewProjection, effectTex_[3]);
		}
		
	}
}

void Effect::SetEffect() {

	switch (effectType_) {
	default:
	case Up:

		for (int i = 0; i < kMaxParticles; i++) {

			worldTransforms_[i].translation_ = startPosition_[i];

			velocities_[i].x = cosf(i * 3.14f / 5.0f) / 3.0f;
			velocities_[i].y = 3.0f + float(rand() % 5) + float((rand() % 10) / 10.0f);
			velocities_[i].z = sinf(i * 3.14f / 5.0f) / 3.0f;

			velocities_[i] *= 0.5f;

			worldTransforms_[i].scale_ = Vector3(0.5f, 0.5f, 0.5f);

			worldTransforms_[i].UpdateMatrix();
		}

		worldTransformPillar_.translation_ = startPosition_[0];
		worldTransformPillar_.scale_ = {5.0f, 5.0f, 5.0f};

		worldTransformPillar_.UpdateMatrix();

		break;
	case Crash:

		for (int i = 0; i < kMaxParticles; i++) {

			worldTransforms_[i].translation_ = startPosition_[i];

			if (i % 2 == 0) {

				velocities_[i].x = cosf(i * 3.14f / 5.0f);
				velocities_[i].y = 3.0f + float((rand() % 10) / 10.0f);
				velocities_[i].z = sinf(i * 3.14f / 5.0f);

				velocities_[i] *= 0.5f;

			} else {

				velocities_[i].x = cosf(i * 3.14f / 5.0f);
				velocities_[i].y = 2.0f + float((rand() % 10) / 10.0f);
				velocities_[i].z = sinf(i * 3.14f / 5.0f);

				velocities_[i] *= 0.5f;
			}

			worldTransforms_[i].scale_ = Vector3(1.0f, 1.0f, 1.0f);

			worldTransforms_[i].UpdateMatrix();
		}

		break;
	case Dust:

		for (int i = 0; i < kMaxParticles; i++) {
			SetRandom();

			velocities_[i].x = float(rand() % 10 - 5) / 100.0f;
			velocities_[i].y = 1.0f;
			velocities_[i].z = float(rand() % 10 - 5) / 100.0f;

			worldTransforms_[i].rotation_ += Vector3(0.3f, 0.0f, 0.0f);

			worldTransforms_[i].translation_ = startPosition_[i];

			velocities_[i] *= 0.5f;

			worldTransforms_[i].scale_ = Vector3(0.5f, 0.5f, 0.5f);

			worldTransforms_[i].UpdateMatrix();
		}

		break;
	}
}

void Effect::Reset(int32_t lifeTime) {
	lifeTimer_ = lifeTime;
	isDead_ = false;
}
