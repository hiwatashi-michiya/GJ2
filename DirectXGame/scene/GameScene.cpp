#include "GameScene.h"
#include "Rand.h"
#include "TextureManager.h"
#include <cassert>
#include <stdlib.h>

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
	gameSpeed_ = GameSpeed::GetInstance();
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
	guardEffectModel_.reset(Model::CreateFromOBJ("guardEffect", true));

	// テクスチャ
	enemyTex_ = TextureManager::Load("king/king.png");
	redTex_ = TextureManager::Load("player/red.png");
	greenTex_ = TextureManager::Load("player/green.png");
	blueTex_ = TextureManager::Load("player/blue.png");
	numberTex_ = TextureManager::Load("UI/commandUI.png");
	alphaRedTex_ = TextureManager::Load("crosseffect/effectred.png");
	alphaBlueTex_ = TextureManager::Load("guardEffect/effectblue.png");
	alphaDarkTex_ = TextureManager::Load("crosseffect/effectdark.png");
	baseBackTex_ = TextureManager::Load("UI/baseBack.png");
	numPlateTex_ = TextureManager::Load("UI/numberPlateBlue.png");
	playerAttackMassTex_ = TextureManager::Load("ground/playerattackmass.png");
	enemyAttackMassTex_ = TextureManager::Load("ground/enemyattackmass.png");
	moveMassTex_ = TextureManager::Load("ground/movemass.png");
	frameTex_ = TextureManager::Load("UI/frame.png");
	nextTex_ = TextureManager::Load("UI/nextUI.png");
	clearTex_ = TextureManager::Load("UI/clear.png");
	gameoverTex_ = TextureManager::Load("UI/gameover.png");
	petalTex_ = TextureManager::Load("UI/petal.png");

	// SE
	damageSE_ = audio_->LoadWave("SE/damage.wav");
	damageHandle_ = damageSE_;

	clearSprite_.reset(Sprite::Create(clearTex_, {0.0f, 0.0f}));
	clearSprite_->SetSize({1280.0f, 720.0f});
	clearSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1280.0f, 720.0f});
	clearSprite_->SetPosition({0.0f, 0.0f});

	gameoverSprite_.reset(Sprite::Create(gameoverTex_, {0.0f, 0.0f}));
	gameoverSprite_->SetSize({1280.0f, 720.0f});
	gameoverSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1280.0f, 720.0f});
	gameoverSprite_->SetPosition({0.0f, 0.0f});

	SetRandom();
	for (uint32_t i = 0; i < 100; i++) {
		randPos[i].x = float(rand() % 14 - 7);
		randPos[i].y = float(rand() % 8 + 3);
	}

	for (uint32_t i = 0; i < 100; i++) {
		petalSprite_[i].reset(Sprite::Create(petalTex_, {0.0f, 0.0f}));
		petalSprite_[i]->SetSize({8.0f, 8.0f});
		petalSprite_[i]->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {8.0f, 8.0f});
		petalSprite_[i]->SetPosition({float(rand() % 1280), -10.0f});
		isPetalDead[i] = false;
	}
	playerTex_ = TextureManager::Load("pawn/pawn.png");
	playerSprite_.reset(Sprite::Create(playerTex_, {0.0f, 0.0f}));
	playerModel_.reset(Model::CreateFromOBJ("pawn", true));

	std::vector<Model*> playerModels{
	    playerModel_.get(), crossEffectModel_.get(), guardEffectModel_.get()};
	std::vector<uint32_t> playerTextures{playerTex_, redTex_,      greenTex_,    blueTex_,
	                                     numberTex_, alphaRedTex_, numPlateTex_, frameTex_,
	                                     nextTex_,   alphaBlueTex_};
	std::vector<uint32_t> playerSounds{damageSE_, damageHandle_};

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures, playerSounds);

	enemyModel_.reset(Model::CreateFromOBJ("king", true));
	std::vector<Model*> enemyModels{
	    enemyModel_.get(), crossEffectModel_.get(), guardEffectModel_.get()};
	std::vector<uint32_t> enemyTextures{enemyTex_,  redTex_,       greenTex_, blueTex_,
	                                    numberTex_, alphaDarkTex_, frameTex_};
	std::vector<uint32_t> enemySounds{damageSE_, damageHandle_};

	// 攻撃マスのワールドトランスフォーム初期化
	for (int z = 0; z < kMaxGrid; z++) {

		for (int x = 0; x < kMaxGrid; x++) {

			effectMass_[z][x].Initialize();
			effectMass_[z][x].SetEffectType(Crash);
			effectMass_[z][x].SetStartPosition(
			    Vector3(-25.0f + x * 10.0f, 1.0f, 25.0f + z * -10.0f));

			worldTransformAttackMass_[z][x].Initialize();
			worldTransformAttackMass_[z][x].scale_ *= 5.0f;
			worldTransformAttackMass_[z][x].translation_ =
			    Vector3(-25.0f + x * 10.0f, 1.0f, 25.0f + z * -10.0f);
			worldTransformAttackMass_[z][x].UpdateMatrix();
		}
	}

	/*for (int z = 0; z < 6; z++) {

	    for (int x = 0; x < 6; x++) {

	        Enemy* newEnemy = new Enemy();
	        newEnemy->Initialize(enemyModels, enemyTextures);
	        newEnemy->SetPlayer(player_.get());
	        newEnemy->SetPosition(x, z);
	        enemies_.push_back(newEnemy);

	    }

	}*/

	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(enemyModels, enemyTextures, enemySounds);
	newEnemy->SetPlayer(player_.get());
	newEnemy->SetGridPosition(3, 3);
	enemies_.push_back(newEnemy);

	Enemy* newEnemy2 = new Enemy();
	newEnemy2->Initialize(enemyModels, enemyTextures, enemySounds);
	newEnemy2->SetPlayer(player_.get());
	newEnemy2->SetGridPosition(5, 5);
	enemies_.push_back(newEnemy2);

	// オプション 初期化
	option->Initialize();

	whiteTex_ = TextureManager::Load("player/player.png");
	blackTex_ = TextureManager::Load("enemy/enemy.png");

	// BGM
	gameBGM_ = audio_->LoadWave("BGM/Battle1.wav");
}

void GameScene::Update() {

	// XINPUT_STATE joyState;

#ifdef _DEBUG

	ImGui::Begin("debug command");
	ImGui::Text("0 key : player HP -= 20");
	ImGui::Text("9 key : all enemy HP -= 20");

	if (player_->GetIsPlayerTurn() == false && CheckAllEnemyTurn() == false) {

		ImGui::Text("1, 2, 3 key : set game speed %d", gameSpeed_->GetGameSpeed());

		if (input_->TriggerKey(DIK_1)) {
			gameSpeed_->SetGameSpeed(1);
		}

		if (input_->TriggerKey(DIK_2)) {
			gameSpeed_->SetGameSpeed(2);
		}

		if (input_->TriggerKey(DIK_3)) {
			gameSpeed_->SetGameSpeed(3);
		}
	}

	ImGui::End();

#endif // _DEBUG

	if (transition_->GetIsChangeScene()) {

		// ゲームシーンにフェードインする時、またはゲームシーンからフェードアウトする時更新
		
		if ((transition_->GetFadeIn() && transition_->GetNextScene() == TITLE) ||
		    (transition_->GetFadeIn() && transition_->GetNextScene() == RESET) ||
		    (transition_->GetFadeOut() && transition_->GetNextScene() == GAME)) {
			transition_->Update();
		}
		// ゲームシーンからのフェードアウト終了でシーン遷移を止める
		else if (transition_->GetFadeIn() && transition_->GetNextScene() == GAME && stageCount_ == 0) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
			gameHandale_ = audio_->PlayWave(gameBGM_, true, option->m_bgmVol);
		} 
		else if (transition_->GetFadeIn() && transition_->GetNextScene() == GAME) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
		}
		// ゲームシーンへのフェードインが完了したら
		else {
			// 実際に遷移する
			transition_->ChangeScene();
		}
	} else {

		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->GetIsDead()) {

				delete enemy;

				return true;
			}

			return false;
		});

		if (isGameClear_ == false && isGameOver_ == false) {

			if (CheckAllEnemyIsDead() && player_->GetIsPlayerTurn() == false) {
				isGameClear_ = true;
			} else if (player_->GetIsDead() && CheckAllEnemyTurn() == false) {
				isGameOver_ = true;
			}

			player_->Update(viewProjection_, option);

		} else {

			// シーンチェンジ
			if ((input_->TriggerKey(DIK_RETURN) || option->GetActionTrigger(ACT))) {

				transition_->SetIsChangeScene(true);
				// 遷移先のシーンをゲームにする
				if (isGameClear_ && stageCount_ < 2) {
					transition_->SetNextScene(RESET);
					stageCount_++;
				}
				else {
					transition_->SetNextScene(TITLE);
					stageCount_ = 0;
					if (audio_->IsPlaying(gameHandale_)) {
						audio_->StopWave(gameHandale_);
					}
				}

			}
		}

		for (Enemy* enemy : enemies_) {
			enemy->Update(viewProjection_, option);
		}

		// エフェクト配置
		for (int z = 0; z < kMaxGrid; z++) {

			for (int x = 0; x < kMaxGrid; x++) {

				if (effectMass_[z][x].IsDead() == false) {
					effectMass_[z][x].Update();
				}
			}
		}

		if (player_->GetIsPlayerTurn()) {
			player_->MoveTurn();
		} else if (CheckAllEnemyTurn()) {

			for (Enemy* enemy : enemies_) {

				if (enemy->GetIsEnemyTurn() && enemy->GetIsEffect() == false) {
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

		// エフェクト配置
		for (int z = 0; z < kMaxGrid; z++) {

			for (int x = 0; x < kMaxGrid; x++) {

				if (collisionManager_->GetAttackMass(x, z) != 0 && effectMass_[z][x].IsDead()) {
					effectMass_[z][x].Reset(60 / gameSpeed_->GetGameSpeed());
					effectMass_[z][x].SetEffectType(Up);
					effectMass_[z][x].SetEffect();
				}
			}
		}

		if ((input_->TriggerKey(DIK_SPACE))) {
			isGameClear_ = true;
		}

		// ゲームクリア時の画面エフェクト
		if (isGameClear_) {
			SetRandom();
			for (uint32_t i = 0; i < 100; i++) {

				petalSprite_[i]->SetPosition(
				    {petalSprite_[i]->GetPosition().x + randPos[i].x + gameSpeed_->GetGameSpeed(),
				     petalSprite_[i]->GetPosition().y + randPos[i].y + gameSpeed_->GetGameSpeed()});

				if (petalSprite_[i]->GetPosition().x < 0.0f ||
				    petalSprite_[i]->GetPosition().x > 1280.0f ||
				    petalSprite_[i]->GetPosition().y > 720.0f) {
					isPetalDead[i] = false;
					petalSprite_[i]->SetPosition({float(rand() % 1280), -10.0f});
					shuffle(randPos[i].x);
					randPos[i].y = float(rand() % 8 + 3);
					isPetalDead[i] = true;
				}
			}
		}
	}

	option->Update();

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

	for (int z = 0; z < kMaxGrid; z++) {

		for (int x = 0; x < kMaxGrid; x++) {

			if (collisionManager_->GetAttackMass(x, z) == PlayerAttack ||
			    collisionManager_->GetAttackMass(x, z) == PlayerSpecialAttack) {

				groundModel_->Draw(
				    worldTransformAttackMass_[z][x], viewProjection_, playerAttackMassTex_);
			}

			if (collisionManager_->GetAttackMass(x, z) == EnemyAttack ||
			    collisionManager_->GetAttackMass(x, z) == EnemySpecialAttack) {

				groundModel_->Draw(
				    worldTransformAttackMass_[z][x], viewProjection_, enemyAttackMassTex_);
			}

			if (collisionManager_->GetAttackMass(x, z) != 0) {
				effectMass_[z][x].Draw(viewProjection_);
			}
		}
	}

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

	if (player_->GetIsStart()) {

		player_->DrawUI();
	}

	for (Enemy* enemy : enemies_) {

		if (enemy->GetIsStart()) {
			enemy->DrawUI();
		}
	}

	if (isGameClear_) {
		clearSprite_->Draw();
		for (uint32_t i = 0; i < 100; i++) {
			if (isPetalDead[i]) {
				petalSprite_[i]->Draw();
			}
		}
	}
	if (isGameOver_) {
		gameoverSprite_->Draw();
	}

	option->Draw();

	// 画面遷移の描画
	transition_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

// 画面遷移
// void GameScene::AddStageTransition() {
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

	// 一人でも生きていたらfalseを返す
	for (Enemy* enemy : enemies_) {

		if (enemy->GetIsDead() == false) {
			return false;
		}
	}

	return true;
}

void GameScene::Reset() {

	isGameClear_ = false;
	isGameOver_ = false;

	for (int z = 0; z < kMaxGrid; z++) {

		for (int x = 0; x < kMaxGrid; x++) {

			collisionManager_->RemoveCollision(x, z);

		}

	}

	player_->Reset();

	enemies_.remove_if([](Enemy* enemy) {
		delete enemy;
		return true;
	});

	std::vector<Model*> enemyModels{
	    enemyModel_.get(), crossEffectModel_.get(), guardEffectModel_.get()};
	std::vector<uint32_t> enemyTextures{enemyTex_,  redTex_,       greenTex_, blueTex_,
	                                    numberTex_, alphaDarkTex_, frameTex_};
	std::vector<uint32_t> enemySounds{damageSE_, damageHandle_};

	Enemy* newEnemy;
	Enemy* newEnemy2;

	switch (stageCount_) {
	default:
	case 0:

		newEnemy = new Enemy();
		newEnemy->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy->SetPlayer(player_.get());
		newEnemy->SetGridPosition(1, 1);
		enemies_.push_back(newEnemy);

		

		break;
	case 1:

		newEnemy2 = new Enemy();
		newEnemy2->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy2->SetPlayer(player_.get());
		newEnemy2->SetGridPosition(2, 4);
		enemies_.push_back(newEnemy2);

		break;
	case 2:

		newEnemy = new Enemy();
		newEnemy->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy->SetPlayer(player_.get());
		newEnemy->SetGridPosition(3, 3);
		enemies_.push_back(newEnemy);

		newEnemy2 = new Enemy();
		newEnemy2->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy2->SetPlayer(player_.get());
		newEnemy2->SetGridPosition(5, 5);
		enemies_.push_back(newEnemy2);

		break;
		

	}

	// 演出上の初期位置
	Vector3 height{0.0f, 100.0f, 0.0f};

	player_->SetWorldPosition(Add(player_->GetPosition(), height));

	for (Enemy* enemy : enemies_) {

		enemy->SetWorldPosition(Add(enemy->GetPosition(), height));
	}

	for (uint32_t i = 0; i < 100; i++) {
		isPetalDead[i] = false;
		petalSprite_[i]->SetPosition({float(rand() % 1280), -10.0f});
	}
}

void GameScene::shuffle(float array) {
	int tmp;
	tmp = int(array);
	array = float(tmp);
}