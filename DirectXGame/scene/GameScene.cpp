#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

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

	// ビュープロジェクション初期化
	viewProjection_.Initialize();
	viewProjection_.farZ = 2000.0f;
	viewProjection_.translation_.y = 70.0f;
	viewProjection_.translation_.z = -70.0f;
	viewProjection_.rotation_.x = 3.14f / 4.0f;
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
	numberTex_ = TextureManager::Load("UI/command.png");
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

	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModels, enemyTextures);
	newEnemy->SetPlayer(player_.get());
	newEnemy->SetPosition(5, 2);
	enemies_.push_back(newEnemy);

	Enemy* newEnemy2 = new Enemy();
	newEnemy2->Initialize(enemyModels, enemyTextures);
	newEnemy2->SetPlayer(player_.get());
	newEnemy2->SetPosition(5, 3);
	enemies_.push_back(newEnemy2);

	// オプション 初期化
	option->Initialize();

	whiteTex_ = TextureManager::Load("player/player.png");
	blackTex_ = TextureManager::Load("enemy/enemy.png");
	AddStageTransition();
}

void GameScene::Update() {

	XINPUT_STATE joyState;

	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {

			delete enemy;
			return true;
		}

		return false;
	});

	player_->Update(option);

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	if (player_->GetIsPlayerTurn()) {
		player_->MoveTurn();
	}
	else if(CheckAllEnemyTurn()) {

		for (Enemy* enemy : enemies_) {

			if (enemy->GetIsEnemyTurn()) {
				enemy->MoveTurn();
				break;
			}

		}

	}
	//全員のターンが終了したらコマンドを打てるようにする
	else {

		if (player_->GetIsSelect() == false) {
			player_->SetIsSelect(true);

			for (Enemy* enemy : enemies_) {
				enemy->SetIsSelect(true);
			}

		}

	}
	
	option->Update(viewProjection_);
	// ビュープロジェクション更新
	viewProjection_.UpdateMatrix();

	if (input_->GetJoystickState(0, joyState)) {
		if ((input_->PushKey(DIK_LEFT) || option->GetActionTrigger(DASH))) {
			AddStageTransition();
		} 
		else if ((input_->PushKey(DIK_RIGHT) || option->GetActionTrigger(JUMP))) {
			isStageTransition_ = false;
		}
	}
	if (isStageTransition_) {
		transition_->Update();
	}
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

	player_->Draw(viewProjection_);

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

	if (isStageTransition_) {
		transition_->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

// 画面遷移
void GameScene::AddStageTransition() {
	std::vector<uint32_t> transitionTextures{whiteTex_, blackTex_};
	transition_ = std::make_unique<TransitionEffect>();
	transition_->Initialize(transitionTextures);
	isStageTransition_ = true;
}

bool GameScene::CheckAllEnemyTurn() {

	for (Enemy* enemy : enemies_) {

		if (enemy->GetIsEnemyTurn()) {
			return true;
		}
	}

	return false;

}
