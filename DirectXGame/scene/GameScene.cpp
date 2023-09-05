#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	playerTex_ = TextureManager::Load("player/player.png");
	playerSprite_.reset(Sprite::Create(playerTex_, {0.0f, 0.0f}));

	player_ = std::make_unique<Player>();
	player_->Initialize(playerSprite_.get());

	enemyTex_ = TextureManager::Load("player/player.png");
	enemySprite_.reset(Sprite::Create(enemyTex_, {0.0f, 0.0f}));

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemySprite_.get());
}

void GameScene::Update() {
	
	player_->Update();
	enemy_->Update();

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	player_->Draw();
	enemy_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
