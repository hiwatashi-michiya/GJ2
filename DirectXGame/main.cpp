#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "Rand.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "TransitionEffect.h"
#include "WinApp.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	TitleScene* titleScene = nullptr;
	GameScene* gameScene = nullptr;
	SetRandom();

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	uint32_t whiteTex_ = TextureManager::Load("player/player.png");
	uint32_t blackTex_ = TextureManager::Load("enemy/enemy.png");

	// タイトルシーンの初期化
	titleScene = new TitleScene();
	titleScene->Initialize();

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	// 画面遷移アニメーションの初期化
	std::unique_ptr<TransitionEffect> transition_ = std::make_unique<TransitionEffect>();
	std::vector<uint32_t> transitionTextures{whiteTex_, blackTex_};
	transition_->Initialize(transitionTextures);

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		input->GetMousePosition();

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// タイトルシーンの毎フレーム処理
		if (transition_->GetChangeGameScene() == false) {
			titleScene->Update();
		}
		// ゲームシーンの毎フレーム処理
		else if (transition_->GetChangeGameScene()) {
			gameScene->Update();
		}
		// 画面遷移の更新
		//if (titleScene->GetChangeGameScene()) {
			transition_->Update();
		//}
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		// タイトルシーンの描画
		if (transition_->GetChangeGameScene() == false) {
			titleScene->Draw();
		}
		// ゲームシーンの描画
		else if (transition_->GetChangeGameScene()) {
			gameScene->Draw();
		}
		// 画面遷移の描画
		//if (titleScene->GetChangeGameScene()) {
			transition_->Draw();
		//}
		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	SafeDelete(titleScene);
	SafeDelete(gameScene);
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}
