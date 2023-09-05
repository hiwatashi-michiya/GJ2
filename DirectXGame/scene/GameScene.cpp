#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//スプライトの生成
	playerTex_ = TextureManager::Load("player/player.png");
	playerSprite_.reset(Sprite::Create(playerTex_, {0.0f, 0.0f}));

	enemyTex_ = TextureManager::Load("player/player.png");
	enemySprite_.reset(Sprite::Create(enemyTex_, {0.0f, 0.0f}));

	// 3Dモデルの生成
	enemyModel_.reset(Model::Create());

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(playerSprite_.get());

	//敵の生成
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModel_.get(),enemySprite_.get());

	// カメラのビュープロジェクションを自キャラにコピー
	enemy_->SetViewProjection(&viewProjection_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowHeight, WinApp::kWindowWidth);
}

void GameScene::Update() {

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		if (isDebugCameraActive_ != 1) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
	}

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

	enemy_->DrawModel(viewProjection_);

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
	enemy_->DrawSprite();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
