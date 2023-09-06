#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include <memory>
#include "PrimitiveDrawer.h"
#include "Enemy.h"
#include "Skydome.h"
#include "Ground.h"
#include "Option.h"
#include "CollisionManager.h"

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

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = nullptr;
	CollisionManager* collisionManager_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	ViewProjection viewProjection_;

	std::unique_ptr<Player> player_ = nullptr;

	std::unique_ptr<Enemy> enemy_ = nullptr;

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// 地面
	std::unique_ptr<Ground> ground_ = nullptr;
	

	//モデル
	std::unique_ptr<Model> playerModel_;
	std::unique_ptr<Model> enemyModel_;
	std::unique_ptr<Model> crossEffectModel_;
	// 天球モデル
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	// 地面モデル
	std::unique_ptr<Model> groundModel_ = nullptr;

	//画像
	std::unique_ptr<Sprite> playerSprite_;

	uint32_t playerTex_ = 0u;
	uint32_t enemyTex_ = 0u;
	uint32_t redTex_ = 0u;
	uint32_t greenTex_ = 0u;
	uint32_t blueTex_ = 0u;
	uint32_t numberTex_ = 0u;
	uint32_t alphaRedTex_ = 0u;
	uint32_t alphaDarkTex_ = 0u;

	// オプション
	Option* option = new Option;

};
