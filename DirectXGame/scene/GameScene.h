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
#include <list>

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

	//画面遷移
	void AddStageTransition();

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

	std::list<Enemy*> enemies_;

	//全ての敵の行動が終わったかどうか確認するフラグ
	bool CheckAllEnemyTurn();

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// 地面
	std::unique_ptr<Ground> ground_ = nullptr;

	// モデル
	std::unique_ptr<Model> playerModel_;
	std::unique_ptr<Model> enemyModel_;
	std::unique_ptr<Model> crossEffectModel_;
	// 天球モデル
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	// 地面モデル
	std::unique_ptr<Model> groundModel_ = nullptr;

	// 画像
	std::unique_ptr<Sprite> playerSprite_;

	uint32_t playerTex_ = 0u;
	uint32_t enemyTex_ = 0u;
	uint32_t redTex_ = 0u;
	uint32_t greenTex_ = 0u;
	uint32_t blueTex_ = 0u;
	uint32_t whiteTex_ = 0u;
	uint32_t blackTex_ = 0u;
	uint32_t numberTex_ = 0u;
	uint32_t alphaRedTex_ = 0u;
	uint32_t alphaDarkTex_ = 0u;

	// オプション
	Option* option = new Option;

	// 画面遷移アニメーション
	std::unique_ptr<TransitionEffect> transition_ = nullptr;
	bool isStageTransition_ = false;
};
