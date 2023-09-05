#pragma once
#include <Model.h>
#include <ViewProjection.h>
#include <WorldTransform.h>

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

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
