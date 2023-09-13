#include "Skydome.h"
#include <cassert>

Skydome::Skydome() {}

Skydome::~Skydome() {}

void Skydome::Initialize(Model* model) {

	assert(model);

	model_ = model;
	worldTransform_.Initialize();

	skyTex_[0] = TextureManager::Load("skydome/skydome.png");
	skyTex_[1] = TextureManager::Load("skydome/skydome_2.png");
	skyTex_[2] = TextureManager::Load("skydome/skydome_3.png");

	currentTex_ = skyTex_[0];

}

void Skydome::Update() {}

void Skydome::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection, currentTex_);
}
