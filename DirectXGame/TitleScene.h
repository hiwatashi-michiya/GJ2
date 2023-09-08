#pragma once

#include "Audio.h"
#include "CollisionManager.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Ground.h"
#include "Input.h"
#include "Model.h"
#include "Option.h"
#include "Player.h"
#include "PrimitiveDrawer.h"
#include "SafeDelete.h"
#include "Skydome.h"
#include "Sprite.h"
#include "TransitionEffect.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <memory>

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// シーン遷移ゲッター
	bool GetChangeGameScene() { return isChangeGameScene_; }

	// シーン遷移セッター
	void SetChangeGameScene(bool flag) { isChangeGameScene_ = flag; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = nullptr;
	CollisionManager* collisionManager_ = nullptr;
	// 画面遷移アニメーション
	TransitionEffect* transition_ = nullptr;

	ViewProjection viewProjection_;

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// 地面
	std::unique_ptr<Ground> ground_ = nullptr;

	//モデル
	std::unique_ptr<Model> crossEffectModel_;
	// 天球モデル
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	// 地面モデル
	std::unique_ptr<Model> groundModel_ = nullptr;

	// 画像
	uint32_t redTex_ = 0u;
	uint32_t greenTex_ = 0u;
	uint32_t blueTex_ = 0u;
	uint32_t whiteTex_ = 0u;
	uint32_t blackTex_ = 0u;
	uint32_t numberTex_ = 0u;
	uint32_t titleTex_ = 0u;

	//SE
	uint32_t debugSE_ = 0u;

	// スプライト
	std::unique_ptr<Sprite> titleSprite_;

	// オプション
	Option* option = new Option;

	bool isChangeGameScene_ = false;

};