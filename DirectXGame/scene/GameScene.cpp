#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	primitiveDrawer_ = PrimitiveDrawer::GetInstance();

	//ビュープロジェクション初期化
	viewProjection_.Initialize();
	viewProjection_.farZ = 2000.0f;
	viewProjection_.translation_.y = 100.0f;
	viewProjection_.translation_.z = -100.0f;
	viewProjection_.rotation_.x = 3.14f / 4.0f;
	//3Dライン描画のビュープロジェクション設定
	primitiveDrawer_->SetViewProjection(&viewProjection_);

	redTex_ = TextureManager::Load("player/red.png");
	greenTex_ = TextureManager::Load("player/green.png");
	blueTex_ = TextureManager::Load("player/blue.png");
	numberTex_ = TextureManager::Load("UI/number.png");

	playerTex_ = TextureManager::Load("pawn/pawn.png");
	playerSprite_.reset(Sprite::Create(playerTex_, {0.0f, 0.0f}));
	playerModel_.reset(Model::CreateFromOBJ("pawn",true));

	std::vector<Model*> playerModels{playerModel_.get()};
	std::vector<uint32_t> playerTextures{playerTex_, redTex_, greenTex_, blueTex_, numberTex_};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures);

	// オプション 初期化
	option->Initialize();
}

void GameScene::Update() {

	player_->Update();

	option->Update(viewProjection_);
	// ビュープロジェクション更新
	viewProjection_.UpdateMatrix();

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

	for (int i = 0; i < 11; i++) {

		float distance = 10 * i - 50.0f;
		Vector3 lineStartX{-50.0f, 0.0f, distance};
		Vector3 lineEndX{50.0f, 0.0f, distance};
		Vector3 lineStartZ{distance, 0.0f, -50.0f};
		Vector3 lineEndZ{distance, 0.0f, 50.0f};
		primitiveDrawer_->DrawLine3d(lineStartX, lineEndX, {1.0f, 1.0f, 1.0f, 1.0f});
		primitiveDrawer_->DrawLine3d(lineStartZ, lineEndZ, {1.0f, 1.0f, 1.0f, 1.0f});
	}

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
	player_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	player_->DrawUI();

	option->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
