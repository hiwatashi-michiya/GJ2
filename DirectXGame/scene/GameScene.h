#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "AxisIndicator.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include "Player.h"
#include "Enemy.h"
#include <memory>
#include "PrimitiveDrawer.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

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

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 画像
	std::unique_ptr<Sprite> playerSprite_;
	uint32_t playerTex_ = 0u;
	std::unique_ptr<Sprite> enemySprite_;
	uint32_t enemyTex_ = 0u;
	uint32_t redTex_ = 0u;
	uint32_t greenTex_ = 0u;
	uint32_t blueTex_ = 0u;
	uint32_t numberTex_ = 0u;

	// 3Dモデル
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> enemyModel_ = nullptr;

	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<Enemy> enemy_ = nullptr;

};
