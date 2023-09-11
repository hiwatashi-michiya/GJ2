#pragma once
#include <WorldTransform.h>
#include <ViewProjection.h>
#include <Model.h>
#include <memory>

const int kMaxParticles = 10;

enum EffectType {
	Up,
	Crash
	
};

class Effect {
public:

	void Initialize();

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void SetEffect();

	bool IsDead() const { return isDead_; }

	void SetEffectType(EffectType type) { effectType_ = type; }

	void Reset(int32_t lifeTime);

	void SetStartPosition(Vector3 position) {

		for (int i = 0; i < kMaxParticles; i++) {
			startPosition_[i] = position;
		}

	}

private:

	std::unique_ptr<Model> effectModel_;

	WorldTransform worldTransforms_[kMaxParticles];
	Vector3 velocities_[kMaxParticles];
	Vector3 startPosition_[kMaxParticles];

	//生存時間
	int32_t lifeTimer_;

	bool isDead_ = true;

	EffectType effectType_ = Up;

	uint32_t effectTex_[2];

};