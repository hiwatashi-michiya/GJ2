#include "TitleScene.h"
#include "TextureManager.h"
#include <cassert>

/// <summary>
/// コンストクラタ
/// </summary>
TitleScene::TitleScene() {}

/// <summary>
/// デストラクタ
/// </summary>
TitleScene::~TitleScene() {}

/// <summary>
/// 初期化
/// </summary>
void TitleScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	primitiveDrawer_ = PrimitiveDrawer::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();
	transition_ = TransitionEffect::GetInstance();

	// ビュープロジェクション初期化
	viewProjection_.Initialize();
	viewProjection_.farZ = 2000.0f;
	viewProjection_.translation_.y = 70.0f;
	viewProjection_.translation_.z = -40.0f;
	viewProjection_.rotation_.x = 3.14f / 3.0f;
	// 3Dライン描画のビュープロジェクション設定
	primitiveDrawer_->SetViewProjection(&viewProjection_);
	// 天球初期化
	skydomeModel_.reset(Model::CreateFromOBJ("skydome", true));
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());
	// 地面初期化
	groundModel_.reset(Model::CreateFromOBJ("ground", true));
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(groundModel_.get());
	crossEffectModel_.reset(Model::CreateFromOBJ("crosseffect", true));

	redTex_ = TextureManager::Load("player/red.png");
	greenTex_ = TextureManager::Load("player/green.png");
	blueTex_ = TextureManager::Load("player/blue.png");
	numberTex_ = TextureManager::Load("UI/command.png");
	whiteTex_ = TextureManager::Load("player/player.png");
	blackTex_ = TextureManager::Load("enemy/enemy.png");
	titleTex_ = TextureManager::Load("UI/title.png");

	debugSE_ = audio_->LoadWave("SE/debugmode.wav");

	titleSprite_.reset(Sprite::Create(titleTex_, {0.0f, 0.0f}));
	titleSprite_->SetSize({1280.0f, 720.0f});
	titleSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1280.0f, 720.0f});
	titleSprite_->SetPosition({0.0f, 0.0f});

	// オプション 初期化
	option->Initialize();
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void TitleScene::Update() {

	// XINPUT_STATE joyState;

	// シーンチェンジ中の処理
	if (transition_->GetIsChangeScene()) {

		// ゲームシーンにフェードインする時、またはゲームシーンからフェードアウトする時更新
		if ((transition_->GetFadeIn() && transition_->GetNextScene() == GAME) ||
		    (transition_->GetFadeOut() && transition_->GetNextScene() == TITLE)) {
			transition_->Update();
		}
		// ゲームシーンからのフェードアウト終了でシーン遷移を止める
		else if (transition_->GetFadeIn() && transition_->GetNextScene() == TITLE) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
		}
		// ゲームシーンへのフェードインが完了したら
		else {
			// 実際に遷移する
			transition_->ChangeScene();
		}

	}
	// シーンチェンジしていない時の処理
	else {

		option->Update();

		// シーンチェンジ
		if ((input_->TriggerKey(DIK_RETURN) || option->GetActionTrigger(ACT))) {
			transition_->SetIsChangeScene(true);
			// 遷移先のシーンをゲームにする
			transition_->SetNextScene(GAME);
			// audio_->PlayWave(debugSE_);
		}
	}

	// ビュープロジェクション更新
	viewProjection_.UpdateMatrix();
}

/// <summary>
/// 描画
/// </summary>
void TitleScene::Draw() {
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

	skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	option->Draw();

	// タイトルの表示
	titleSprite_->Draw();

	// 画面遷移の描画
	transition_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
