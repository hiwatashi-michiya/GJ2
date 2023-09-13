#pragma once
#include <Model.h>
#include <ViewProjection.h>
#include <WorldTransform.h>
#include <TextureManager.h>

class Skydome {
public:
	Skydome();
	~Skydome();

	// 初期化
	void Initialize(Model* model);

	// 更新
	void Update();

	// 描画
	void Draw(const ViewProjection& viewProjection);

	void SetTex(int num) {

		num = IntClamp(num, 0, 2);

		currentTex_ = skyTex_[num];

	}

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	uint32_t currentTex_ = 0u;

	uint32_t skyTex_[3];

};
