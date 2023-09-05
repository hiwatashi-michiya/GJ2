#include "Ground.h"
#include <cassert>

Ground::Ground() {}

Ground::~Ground() {}

// 初期化
void Ground::Initialize(Model* model) {

	assert(model);

	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale_.x = 30.0f;
	worldTransform_.scale_.y = 30.0f;
	worldTransform_.scale_.z = 30.0f;
	worldTransform_.UpdateMatrix();
}

// 更新
void Ground::Update() {}

// 描画
void Ground::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection);
}
