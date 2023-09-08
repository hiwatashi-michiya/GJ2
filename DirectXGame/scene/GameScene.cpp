#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#ifdef _DEBUG
#include <ImGuiManager.h>
#endif // _DEBUG

GameScene::GameScene() {}

GameScene::~GameScene() {

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

}

void GameScene::Initialize() {

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
	// ビュープロジェクション更新
	viewProjection_.UpdateMatrix();
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

	enemyTex_ = TextureManager::Load("king/king.png");
	redTex_ = TextureManager::Load("player/red.png");
	greenTex_ = TextureManager::Load("player/green.png");
	blueTex_ = TextureManager::Load("player/blue.png");
	numberTex_ = TextureManager::Load("UI/commandUI.png");
	alphaRedTex_ = TextureManager::Load("crosseffect/effectred.png");
	alphaDarkTex_ = TextureManager::Load("crosseffect/effectdark.png");

	playerTex_ = TextureManager::Load("pawn/pawn.png");
	playerSprite_.reset(Sprite::Create(playerTex_, {0.0f, 0.0f}));
	playerModel_.reset(Model::CreateFromOBJ("pawn", true));

	std::vector<Model*> playerModels{playerModel_.get(), crossEffectModel_.get()};
	std::vector<uint32_t> playerTextures{playerTex_, redTex_, greenTex_, blueTex_, numberTex_, alphaRedTex_};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures);

	enemyModel_.reset(Model::CreateFromOBJ("king", true));
	std::vector<Model*> enemyModels{enemyModel_.get(), crossEffectModel_.get()};
	std::vector<uint32_t> enemyTextures{enemyTex_, redTex_, greenTex_, blueTex_, numberTex_, alphaDarkTex_};

	for (int z = 0; z < 6; z++) {

		for (int x = 0; x < 6; x++) {

			Enemy* newEnemy = new Enemy();
			newEnemy->Initialize(enemyModels, enemyTextures);
			newEnemy->SetPlayer(player_.get());
			newEnemy->SetPosition(x, z);
			enemies_.push_back(newEnemy);

		}

	}

	

	/*Enemy* newEnemy2 = new Enemy();
	newEnemy2->Initialize(enemyModels, enemyTextures);
	newEnemy2->SetPlayer(player_.get());
	newEnemy2->SetPosition(5, 5);
	enemies_.push_back(newEnemy2);*/

	// オプション 初期化
	option->Initialize();

	whiteTex_ = TextureManager::Load("player/player.png");
	blackTex_ = TextureManager::Load("enemy/enemy.png");
	
}

void GameScene::Update() {

	//XINPUT_STATE joyState;

#ifdef _DEBUG

	ImGui::Begin("debug command");
	ImGui::Text("1 key : player HP -= 20");
	ImGui::Text("2 key : all enemy HP -= 20");
	ImGui::End();

#endif // _DEBUG

	XINPUT_STATE joyState;

	if (transition_->GetIsChangeScene()) {

		// ゲームシーンにフェードインする時、またはゲームシーンからフェードアウトする時更新
		if ((transition_->GetFadeIn() && transition_->GetNextScene() == TITLE) ||
		    (transition_->GetFadeOut() && transition_->GetNextScene() == GAME)) {
			transition_->Update();
		}
		// ゲームシーンからのフェードアウト終了でシーン遷移を止める
		else if (transition_->GetFadeIn() && transition_->GetNextScene() == GAME) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
		}
		// ゲームシーンへのフェードインが完了したら
		else {
			// 実際に遷移する
			transition_->ChangeScene();
		}
	}
	else {

		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->GetIsDead()) {

				delete enemy;
				return true;
			}

			return false;
		});

		if (isGameClear_ == false && isGameOver_ == false) {

			if (CheckAllEnemyIsDead()) {
				isGameClear_ = true;
			} else if (player_->GetIsDead()) {
				isGameOver_ = true;
			}

			player_->Update(option);

		} else {

			// シーンチェンジ
			if (input_->GetJoystickState(0, joyState)) {
				if ((input_->PushKey(DIK_LEFT) || option->GetActionTrigger(DASH))) {
					transition_->SetIsChangeScene(true);
					// 遷移先のシーンをゲームにする
					transition_->SetNextScene(TITLE);
				}
			}

		}

		for (Enemy* enemy : enemies_) {
			enemy->Update(viewProjection_);
		}

		if (player_->GetIsPlayerTurn()) {
			player_->MoveTurn();
		} else if (CheckAllEnemyTurn()) {

			for (Enemy* enemy : enemies_) {

				if (enemy->GetIsEnemyTurn()) {
					enemy->MoveTurn(viewProjection_);
					break;
				}
			}

		}
		// 全員のターンが終了したらコマンドを打てるようにする
		else {

			if (player_->GetIsSelect() == false) {
				player_->SetIsSelect(true);

				for (Enemy* enemy : enemies_) {
					enemy->SetIsSelect(true);
				}
			}
		}

	}
	
	option->Update(viewProjection_);

	// ビュープロジェクション更新
	viewProjection_.UpdateMatrix();

	/*if (input_->GetJoystickState(0, joyState)) {
		if ((input_->PushKey(DIK_LEFT) || option->GetActionTrigger(DASH))) {
			AddStageTransition();
		} 
		else if ((input_->PushKey(DIK_RIGHT) || option->GetActionTrigger(JUMP))) {
			isStageTransition_ = false;
		}
	}
	if (isStageTransition_) {
		transition_->Update();
	}*/
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

	skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);

	if (player_->GetIsDead() == false) {
		player_->Draw(viewProjection_);
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}

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

	for (Enemy* enemy : enemies_) {
		enemy->DrawUI();
	}

	option->Draw();

	// 画面遷移の描画
	transition_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

// 画面遷移
//void GameScene::AddStageTransition() {
//	std::vector<uint32_t> transitionTextures{whiteTex_, blackTex_};
//	transition_ = std::make_unique<TransitionEffect>();
//	transition_->Initialize(transitionTextures);
//	isStageTransition_ = true;
//}

bool GameScene::CheckAllEnemyTurn() {

	for (Enemy* enemy : enemies_) {

		if (enemy->GetIsEnemyTurn()) {
			return true;
		}
	}

	return false;

}


bool GameScene::CheckAllEnemyIsDead() {

	//一人でも生きていたらfalseを返す
	for (Enemy* enemy : enemies_) {

		if (enemy->GetIsDead() == false) {
			return false;
		}
	}

	return true;

}
