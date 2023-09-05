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
#include "Option.h"

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

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	ViewProjection viewProjection_;

	std::unique_ptr<Player> player_ = nullptr;

	//モデル
	std::unique_ptr<Model> playerModel_;

	//画像
	std::unique_ptr<Sprite> playerSprite_;

	uint32_t playerTex_ = 0u;
	uint32_t redTex_ = 0u;
	uint32_t greenTex_ = 0u;
	uint32_t blueTex_ = 0u;
	uint32_t numberTex_ = 0u;

	// オプション
	Option* option = new Option;

};
