#pragma once
#include <WorldTransform.h>
#include <ViewProjection.h>
#include <Model.h>
#include <memory>

const int kMaxParticles = 10;

enum EffectType {
	Up,
	Crash,
	Move,
	Dust,

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

	void SetTexture(int num, uint32_t texture) {
		num = IntClamp(num, 0, 2);
		effectTex_[num] = texture; 
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

	uint32_t effectTex_[3];

};
