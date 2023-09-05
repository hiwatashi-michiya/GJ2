#pragma once
#include "MoveCommand.h"
#include <Input.h>
#include <Sprite.h>
#include <TextureManager.h>
#include <Vector3.h>
#include <memory>
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class Enemy : public MoveCommand {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sprite">スプライト</param>
	void Initialize(Model* model, Sprite* sprite);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void DrawSprite();
	void DrawModel(ViewProjection& viewProjection);

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// モデル
	Model* model_ = nullptr;

	// 入力
	Input* input_ = nullptr;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	// 描画位置(中央)
	Vector3 position_;

	// 速度
	Vector3 spriteVelocity_;
	Vector3 modelVelocity_;

	// 現在の行動コマンド
	Command currentMoveCommand_ = Stop;

	// 行動時間
	const int kMoveTime = 60;

	int MoveTimer_ = 0;

	// 行動中かどうか
	bool isMove_ = false;

	// 行動クールタイム
	int kEnemyCoolTime_ = 60;

	int coolTime_ = 0;

	bool isCoolTime = false;

	// 行動
	void Move(Command command);

	// スプライト
	Sprite* enemySprite_;

	// 画像位置設定。ポジションの変更後に使用
	void SetSpritePosition() {
		enemySprite_->SetPosition(
		    {position_.x - kTextureWidth / 2.0f, 
			position_.y - kTextureHeight / 2.0f});
	}

	// 画像横幅
	const int kTextureWidth = 32;
	// 画像縦幅
	const int kTextureHeight = 32;
};
