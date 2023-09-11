#include "Effect.h"
#include "Rand.h"

void Effect::Initialize() {

	effectModel_.reset(Model::CreateFromOBJ("particlecube", true));
	for (int i = 0; i < kMaxParticles; i++) {
		worldTransforms_[i].Initialize();
		worldTransforms_[i].scale_ *= 0.5f;
	}

	effectTex_[0] = TextureManager::Load("particlecube/effect.png");
	effectTex_[1] = TextureManager::Load("particlecube/effect_black.png");

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
				worldTransforms_[i].rotation_ += Vector3(0.3f, 0.0f, 0.0f);
				worldTransforms_[i].UpdateMatrix();
			}

			break;
		case Crash:

			for (int i = 0; i < kMaxParticles; i++) {
				worldTransforms_[i].translation_ += velocities_[i];
				worldTransforms_[i].UpdateMatrix();
			}

			break;
		}

	}

}

void Effect::Draw(const ViewProjection& viewProjection) {

	if (isDead_ == false) {

		for (int i = 0; i < kMaxParticles; i++) {

			effectModel_->Draw(worldTransforms_[i], viewProjection, effectTex_[i % 2]);
		}

	}

}



void Effect::SetEffect() {

	switch (effectType_) {
	default:
	case Up:

		for (int i = 0; i < kMaxParticles; i++) {

			worldTransforms_[i].translation_ = startPosition_[i];

			velocities_[i].x = cosf(i * 3.14f / 5.0f) / 2.0f;
			velocities_[i].y = 3.0f + float(rand() % 5) + float((rand() % 10) / 10.0f);
			velocities_[i].z = sinf(i * 3.14f / 5.0f) / 2.0f;

			velocities_[i] *= 0.5f;

			worldTransforms_[i].UpdateMatrix();

		}

		break;
	case Crash:

		for (int i = 0; i < kMaxParticles; i++) {

			worldTransforms_[i].translation_ = startPosition_[i];

			if (i % 2 == 0) {

				velocities_[i].x = cosf(i * 3.14f / 5.0f);
				velocities_[i].y = 0.5f;
				velocities_[i].z = sinf(i * 3.14f / 5.0f);

				velocities_[i] *= 0.1f;

			} else {

				velocities_[i].x = cosf(i * 3.14f / 5.0f);
				velocities_[i].y = 1.0f;
				velocities_[i].z = sinf(i * 3.14f / 5.0f);

				velocities_[i] *= 0.1f;
			}

			worldTransforms_[i].UpdateMatrix();
		}

		break;
	
	}


}

void Effect::Reset(int32_t lifeTime) {
	lifeTimer_ = lifeTime;
	isDead_ = false;
}
