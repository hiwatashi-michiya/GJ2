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
#include "GameSpeed.h"
#include "Effect.h"

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
	//void AddStageTransition();

	// シーン遷移ゲッター
	bool GetChangeGameScene() { return isChangeGameScene_; }

	// シーン遷移セッター
	void SetChangeGameScene(bool flag) { isChangeGameScene_ = flag; }

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	void shuffle(float array);

	bool GetIsReset() { return isReset_; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = nullptr;
	CollisionManager* collisionManager_ = nullptr;
	GameSpeed* gameSpeed_ = nullptr;
	// 画面遷移アニメーション
	TransitionEffect* transition_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	//マスのワールドトランスフォーム
	WorldTransform worldTransformMass_[kMaxGrid][kMaxGrid];
	//エフェクト表示
	Effect effectMass_[kMaxGrid][kMaxGrid];

	ViewProjection viewProjection_;

	std::unique_ptr<Player> player_ = nullptr;

	std::unique_ptr<Enemy> enemy_ = nullptr;

	std::list<Enemy*> enemies_;

	//全ての敵の行動が終わったかどうか確認するフラグ
	bool CheckAllEnemyTurn();

	//全ての敵が死んだかどうか確認するフラグ
	bool CheckAllEnemyIsDead();

	//プレイヤーの行動自前描画
	void PredictionActDraw();

	// 天球
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// 地面
	std::unique_ptr<Ground> ground_ = nullptr;

	// モデル
	std::unique_ptr<Model> playerModel_;
	std::unique_ptr<Model> enemyModel_;
	std::unique_ptr<Model> crossEffectModel_;
	std::unique_ptr<Model> guardEffectModel_;
	// 天球モデル
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	// 地面モデル
	std::unique_ptr<Model> groundModel_ = nullptr;

	// 画像
	std::unique_ptr<Sprite> playerSprite_;
	std::unique_ptr<Sprite> clearSprite_;
	std::unique_ptr<Sprite> gameoverSprite_;
	std::unique_ptr<Sprite> petalSprite_[100];

	uint32_t playerTex_ = 0u;
	uint32_t enemyTex_ = 0u;
	uint32_t redTex_ = 0u;
	uint32_t greenTex_ = 0u;
	uint32_t blueTex_ = 0u;
	uint32_t whiteTex_ = 0u;
	uint32_t blackTex_ = 0u;
	uint32_t numberTex_ = 0u;
	uint32_t alphaRedTex_ = 0u;
	uint32_t alphaBlueTex_ = 0u;
	uint32_t alphaDarkTex_ = 0u;
	uint32_t baseBackTex_ = 0u;
	uint32_t numPlateTex_ = 0u;
	uint32_t playerAttackMassTex_ = 0u;
	uint32_t enemyAttackMassTex_ = 0u;
	uint32_t moveMassTex_ = 0u;
	uint32_t enemyMoveMassTex_ = 0u;
	uint32_t frameTex_ = 0u;
	uint32_t nextTex_ = 0u;
	uint32_t clearTex_ = 0u;
	uint32_t gameoverTex_ = 0u;
	uint32_t petalTex_ = 0u;

	// BGM
	uint32_t gameBGM_ = 0u;
	uint32_t gameHandale_ = 0u;

	// SE
	uint32_t damageSE_ = 0u;
	uint32_t damageHandle_ = 0u;


	// オプション
	Option* option = Option::GetInstance();

	
	/*bool isStageTransition_ = false;*/

	bool isChangeGameScene_ = false;

	bool isGameClear_ = false;

	bool isGameOver_ = false;

	int stageCount_ = 0;

	bool isReset_ = false;

	Vector2 randPos[100] = {0.0f, 0.0f};
	bool isPetalDead[100];
};
