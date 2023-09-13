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
	arrowLeftModel_.reset(Model::CreateFromOBJ("arrowleft", true));
	arrowRightModel_.reset(Model::CreateFromOBJ("arrowright", true));
	arrowUpModel_.reset(Model::CreateFromOBJ("arrowup", true));
	arrowDownModel_.reset(Model::CreateFromOBJ("arrowdown", true));

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
	enemyMoveMassTex_ = TextureManager::Load("ground/enemymovemass.png");
	frameTex_ = TextureManager::Load("UI/frame.png");
	nextTex_ = TextureManager::Load("UI/nextUI.png");
	gameClearTex_ = TextureManager::Load("UI/clear.png");
	stageClearTex_ = TextureManager::Load("UI/stageClear.png");
	gameoverTex_ = TextureManager::Load("UI/gameover.png");
	petalTex_ = TextureManager::Load("UI/petal.png");
	ctrKeyTex_ = TextureManager::Load("UI/ctrKey.png");
	ctrPadTex_ = TextureManager::Load("UI/ctrPad.png");
	ctrPadGameTex_ = TextureManager::Load("UI/ctrPad_game.png");
	ctrKeyGameTex_ = TextureManager::Load("UI/ctrKey_game.png");
	playerAttackTex_ = TextureManager::Load("attack/playerattack.png");
	enemyAttackTex_ = TextureManager::Load("attack/enemyattack.png");
	playerSpecialAttackTex_ = TextureManager::Load("attack/playerspecialattack.png");
	enemySpecialAttackTex_ = TextureManager::Load("attack/enemyspecialattack.png");
	optionTex_ = TextureManager::Load("UI/option.png");
	AtitleTex_ = TextureManager::Load("UI/Atitle.png");
	AnextTex_ = TextureManager::Load("UI/Anext.png");

	ctrPadSprite_.reset(Sprite::Create(ctrPadTex_, {860.0f, 45.0f}));
	ctrKeySprite_.reset(Sprite::Create(ctrKeyTex_, {860.0f, 45.0f}));
	ctrPadGameSprite_.reset(Sprite::Create(ctrPadGameTex_, {1000.0f, 450.0f}));
	ctrKeyGameSprite_.reset(Sprite::Create(ctrKeyGameTex_, {1000.0f, 450.0f}));

	// スプライト
	ctrSprite_ = ctrKeySprite_.get();
	ctrSprite_->SetSize({240.0f, 720.0f});
	ctrSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {720.0f, 1280.0f});
	ctrSprite_->SetPosition({1040.0f, 0.00f});

	ctrGameSprite_ = ctrKeyGameSprite_.get();
	ctrGameSprite_->SetSize({240.0f, 240.0f});
	ctrGameSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {720.0f, 1024.0f});
	ctrGameSprite_->SetPosition({1000.0f, 450.0f});

	AnextSprite_.reset(Sprite::Create(AnextTex_, {300.0f, 380.0f}));
	AnextSprite_->SetSize({680.0f, 360.0f});
	AnextSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {2560.0f, 1440.0f});

	AtitleSprite_.reset(Sprite::Create(AtitleTex_, {300.0f, 380.0f}));
	AtitleSprite_->SetSize({680.0f, 360.0f});
	AtitleSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {2560.0f, 1440.0f});

	// SE
	damageSE_ = audio_->LoadWave("SE/damage.wav");
	damageHandle_ = damageSE_;
	moveSE_ = audio_->LoadWave("SE/move.wav");
	moveHandle_ = moveSE_;
	optionOpenSE_ = audio_->LoadWave("SE/optionopen.wav");
	optionCloseSE_ = audio_->LoadWave("SE/optionclose.wav");
	guardSE_ = audio_->LoadWave("SE/guard.wav");

	gameClearSprite_.reset(Sprite::Create(gameClearTex_, {0.0f, 0.0f}));
	gameClearSprite_->SetSize({1280.0f, 720.0f});
	gameClearSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1280.0f, 720.0f});
	gameClearSprite_->SetPosition({0.0f, 0.0f});

	stageClearSprite_.reset(Sprite::Create(stageClearTex_, {0.0f, 0.0f}));
	stageClearSprite_->SetSize({1280.0f, 720.0f});
	stageClearSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1280.0f, 720.0f});
	stageClearSprite_->SetPosition({0.0f, 0.0f});

	gameoverSprite_.reset(Sprite::Create(gameoverTex_, {0.0f, 0.0f}));
	gameoverSprite_->SetSize({1280.0f, 720.0f});
	gameoverSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {1280.0f, 720.0f});
	gameoverSprite_->SetPosition({0.0f, 0.0f});

	optionSprite_.reset(Sprite::Create(optionTex_, {1040.0f, 0.0f}));
	optionSprite_->SetSize({240.0f, 100.0f});
	optionSprite_->SetTextureRect(
	    {
	        0.0f,
	        0.0f,
	    },
	    {2560.0f, 1440.0f});

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
	std::vector<uint32_t> playerTextures{playerTex_, redTex_,       greenTex_,    blueTex_,
	                                     numberTex_, alphaRedTex_,  numPlateTex_, frameTex_,
	                                     nextTex_,   alphaBlueTex_, alphaDarkTex_};
	std::vector<uint32_t> playerSounds{damageSE_, damageHandle_, moveSE_, moveHandle_};

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures, playerSounds);

	enemyModel_.reset(Model::CreateFromOBJ("king", true));
	std::vector<Model*> enemyModels{
	    enemyModel_.get(), crossEffectModel_.get(), guardEffectModel_.get()};
	std::vector<uint32_t> enemyTextures{enemyTex_,  redTex_,       greenTex_, blueTex_,
	                                    numberTex_, alphaDarkTex_, frameTex_};
	std::vector<uint32_t> enemySounds{damageSE_, damageHandle_, moveSE_, moveHandle_};

	// マスのワールドトランスフォーム初期化
	for (int z = 0; z < kMaxGrid; z++) {

		for (int x = 0; x < kMaxGrid; x++) {

			effectMass_[z][x].Initialize();
			effectMass_[z][x].SetEffectType(Crash);
			effectMass_[z][x].SetStartPosition(
			    Vector3(-25.0f + x * 10.0f, 1.0f, 25.0f + z * -10.0f));

			worldTransformMass_[z][x].Initialize();
			worldTransformMass_[z][x].scale_ *= 5.0f;
			worldTransformMass_[z][x].translation_ =
			    Vector3(-25.0f + x * 10.0f, 1.0f, 25.0f + z * -10.0f);
			worldTransformMass_[z][x].UpdateMatrix();

			worldTransformArrowMass_[z][x].Initialize();
			worldTransformArrowMass_[z][x].scale_ *= 5.0f;
			worldTransformArrowMass_[z][x].translation_ =
			    Vector3(-25.0f + x * 10.0f, 1.5f, 25.0f + z * -10.0f);
			worldTransformArrowMass_[z][x].UpdateMatrix();
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
	bossBGM_ = audio_->LoadWave("BGM/Battle2.wav");
	clearBGM = audio_->LoadWave("BGM/clear.wav");

}

void GameScene::Update() {

	XINPUT_STATE joyState;

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

	if (input_->GetJoystickState(0, joyState)) {
		ctrSprite_ = ctrPadSprite_.get();
		ctrSprite_->SetSize({300.0f, 628.0f});
		ctrSprite_->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {720.0f, 1280.0f});
		ctrGameSprite_ = ctrPadGameSprite_.get();
		ctrGameSprite_->SetSize({180.0f, 256.0f});
		ctrGameSprite_->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {720.0f, 1024.0f});
		ctrGameSprite_->SetPosition({1090.0f, 450.0f});

	} else {
		ctrSprite_ = ctrKeySprite_.get();
		ctrSprite_->SetSize({300.0f, 628.0f});
		ctrSprite_->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {720.0f, 1280.0f});
		ctrGameSprite_ = ctrKeyGameSprite_.get();
		ctrGameSprite_->SetSize({180.0f, 256.0f});
		ctrGameSprite_->SetTextureRect(
		    {
		        0.0f,
		        0.0f,
		    },
		    {720.0f, 1024.0f});
		ctrGameSprite_->SetPosition({1090.0f, 450.0f});
	}

	if (transition_->GetIsChangeScene()) {

		// ゲームシーンにフェードインする時、またはゲームシーンからフェードアウトする時更新

		if ((transition_->GetFadeIn() && transition_->GetNextScene() == TITLE) ||
		    (transition_->GetFadeIn() && transition_->GetNextScene() == RESET) ||
		    (transition_->GetFadeOut() && transition_->GetNextScene() == GAME)) {
			transition_->Update();
		}
		// ゲームシーンからのフェードアウト終了でシーン遷移を止める
		else if (
		    transition_->GetFadeIn() && transition_->GetNextScene() == GAME && stageCount_ == 0) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();

		} else if (transition_->GetFadeIn() && transition_->GetNextScene() == GAME) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
			if (!audio_->IsPlaying(gameHandale_)) {
				if (stageCount_ < 2) {

					gameHandale_ = audio_->PlayWave(gameBGM_, true, option->m_bgmVol * 0.8f);

				} else if (stageCount_ == 2) {

					gameHandale_ = audio_->PlayWave(bossBGM_, true, option->m_bgmVol * 0.8f);
				}
			}
		}
		// ゲームシーンへのフェードインが完了したら
		else {
			// 実際に遷移する
			transition_->ChangeScene();
		}
	} else {

		if (!isGameClear_ && !isGameOver_)
		{

			if (!audio_->IsPlaying(gameHandale_)) {
				if (stageCount_ < 2) {

					gameHandale_ = audio_->PlayWave(gameBGM_, true, option->m_bgmVol * 0.8f);

				} else if (stageCount_ == 2) {

					gameHandale_ = audio_->PlayWave(bossBGM_, true, option->m_bgmVol * 0.8f);
				}
			}
		}

		// オプション画面時の処理
		if (option->GetMenuOverlay()) {

			if (option->GetActionTrigger(MENU) || input_->TriggerKey(DIK_Q)) {
				option->isMenuOverlay_ = false;
				audio_->PlayWave(optionCloseSE_, false, option->m_seVol * 0.8f);
			}

			audio_->SetVolume(gameHandale_, option->m_bgmVol * 0.8f * 0.5f);

		} else {

			if (option->GetActionTrigger(MENU) || input_->TriggerKey(DIK_Q)) {
				option->isMenuOverlay_ = true;
				audio_->PlayWave(optionOpenSE_, false, option->m_seVol * 0.8f);
			}

			enemies_.remove_if([](Enemy* enemy) {
				if (enemy->GetIsDead()) {

					delete enemy;

					return true;
				}

				return false;
			});

			if (isStageClear_ == false && isGameOver_ == false) {

				if (CheckAllEnemyIsDead() && player_->GetIsPlayerTurn() == false) {
					isStageClear_ = true;

					if (stageCount_ == 2) {
						isGameClear_ = true;

						audio_->StopWave(gameHandale_);
						clearHandale = audio_->PlayWave(clearBGM, false, option->m_bgmVol);
					}

				} else if (player_->GetIsDead() && CheckAllEnemyTurn() == false) {
					isGameOver_ = true;
				}

				player_->Update(viewProjection_);

			} else {

				// シーンチェンジ
				if ((input_->TriggerKey(DIK_RETURN) || option->GetActionTrigger(ACT))) {

					if (stageCount_ == 2 && audio_->IsPlaying(clearHandale)) {
						
					} else {

						audio_->PlayWave(guardSE_, false, option->m_seVol * 1.2f);

						transition_->SetIsChangeScene(true);
						// 遷移先のシーンをゲームにする
						if (isStageClear_ && stageCount_ < 2) {
							transition_->SetNextScene(RESET);
							stageCount_++;

							if (stageCount_ == 2) {
								audio_->StopWave(gameHandale_);
							}

						} else {
							transition_->SetNextScene(TITLE);
							stageCount_ = 0;
							audio_->StopWave(gameHandale_);
							audio_->StopWave(clearHandale);
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

						switch (collisionManager_->GetAttackMass(x, z)) {
						default:
						case 1:
							effectMass_[z][x].SetTexture(3, playerAttackTex_);
							break;
						case 2:
							effectMass_[z][x].SetTexture(3, enemyAttackTex_);
							break;
						case 3:
							effectMass_[z][x].SetTexture(3, playerSpecialAttackTex_);
							break;
						case 4:
							effectMass_[z][x].SetTexture(3, enemySpecialAttackTex_);
							break;
						}
					}
				}
			}
			audio_->SetVolume(gameHandale_, option->m_bgmVol * 0.8f);
		}

		// ゲームクリア時の画面エフェクト
		if (isStageClear_) {
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
		/*if (input_->PushKey(DIK_SPACE)) {
			isStageClear_ = true;
		}*/
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

	PredictionActDraw();

	for (int z = 0; z < kMaxGrid; z++) {

		for (int x = 0; x < kMaxGrid; x++) {

			if (collisionManager_->GetAttackMass(x, z) == PlayerAttack ||
			    collisionManager_->GetAttackMass(x, z) == PlayerSpecialAttack) {

				groundModel_->Draw(
				    worldTransformMass_[z][x], viewProjection_, playerAttackMassTex_);
			}

			if (collisionManager_->GetAttackMass(x, z) == EnemyAttack ||
			    collisionManager_->GetAttackMass(x, z) == EnemySpecialAttack) {

				groundModel_->Draw(worldTransformMass_[z][x], viewProjection_, enemyAttackMassTex_);
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

	ctrGameSprite_->Draw();

	optionSprite_->Draw();

	if (isStageClear_) {
		stageClearSprite_->Draw();
		if (isGameClear_){
			gameClearSprite_->Draw();
		}
		for (uint32_t i = 0; i < 100; i++) {
			if (isPetalDead[i]) {
				petalSprite_[i]->Draw();
			}
		}

		if (stageCount_ == 2 && audio_->IsPlaying(clearHandale)) {

		} else {
			AnextSprite_->Draw();
		}

	}
	if (isGameOver_) {
		gameoverSprite_->Draw();
		AtitleSprite_->Draw();
	}

	option->Draw();

	// オプション画面時の処理
	if (option->GetMenuOverlay()) {
		ctrSprite_->Draw();
	}

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

	isStageClear_ = false;
	isGameClear_ = false;
	isGameOver_ = false;

	for (int z = 0; z < kMaxGrid; z++) {

		for (int x = 0; x < kMaxGrid; x++) {

			collisionManager_->RemoveCollision(x, z);
		}
	}

	player_->Reset();

	skydome_->SetTex(stageCount_);

	enemies_.remove_if([](Enemy* enemy) {
		delete enemy;
		return true;
	});

	std::vector<Model*> enemyModels{
	    enemyModel_.get(), crossEffectModel_.get(), guardEffectModel_.get()};
	std::vector<uint32_t> enemyTextures{enemyTex_,  redTex_,       greenTex_, blueTex_,
	                                    numberTex_, alphaDarkTex_, frameTex_};
	std::vector<uint32_t> enemySounds{damageSE_, damageHandle_, moveSE_, moveHandle_};

	Enemy* newEnemy;
	Enemy* newEnemy2;
	Enemy* newEnemy3;

	switch (stageCount_) {
	default:
	case 0:

		newEnemy = new Enemy();
		newEnemy->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy->SetPlayer(player_.get());
		newEnemy->SetGridPosition(4, 4);
		newEnemy->SetLife(50);
		enemies_.push_back(newEnemy);

		player_->SetGridPosition(1, 1);

		break;
	case 1:

		newEnemy = new Enemy();
		newEnemy->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy->SetPlayer(player_.get());
		newEnemy->SetGridPosition(4, 4);
		newEnemy->SetLife(50);
		enemies_.push_back(newEnemy);

		newEnemy2 = new Enemy();
		newEnemy2->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy2->SetPlayer(player_.get());
		newEnemy2->SetGridPosition(4, 2);
		newEnemy2->SetLife(50);
		enemies_.push_back(newEnemy2);

		player_->SetGridPosition(1, 3);

		break;
	case 2:

		newEnemy = new Enemy();
		newEnemy->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy->SetPlayer(player_.get());
		newEnemy->SetGridPosition(4, 4);
		newEnemy->SetLife(50);
		enemies_.push_back(newEnemy);

		newEnemy2 = new Enemy();
		newEnemy2->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy2->SetPlayer(player_.get());
		newEnemy2->SetGridPosition(1, 1);
		newEnemy2->SetLife(50);
		enemies_.push_back(newEnemy2);

		newEnemy3 = new Enemy();
		newEnemy3->Initialize(enemyModels, enemyTextures, enemySounds);
		newEnemy3->SetPlayer(player_.get());
		newEnemy3->SetGridPosition(4, 1);
		newEnemy3->SetLife(100);
		newEnemy3->SetBossFlag();
		enemies_.push_back(newEnemy3);

		player_->SetGridPosition(1, 4);

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

void GameScene::PredictionActDraw() {

	if (player_->GetIsSelect()) {

		int tmpGridX = player_->GetGridX();
		int tmpGridZ = player_->GetGridZ();

		// 仮の当たり判定
		int32_t tmpCollisionMass[kMaxGrid][kMaxGrid]{};

		tmpCollisionMass[tmpGridZ][tmpGridX] = 1;

		for (Enemy* enemy : enemies_) {
			tmpCollisionMass[enemy->GetGridZ()][enemy->GetGridX()] = 1;
		}

		for (Command command : player_->moveCommands_) {

			switch (command) {
			case MoveLeft:

				if (tmpGridX - 1 >= 0 && tmpCollisionMass[tmpGridZ][tmpGridX - 1] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridX -= 1;
					arrowLeftModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case MoveRight:

				if (tmpGridX + 1 < 6 && tmpCollisionMass[tmpGridZ][tmpGridX + 1] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridX += 1;
					arrowRightModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case MoveUp:

				if (tmpGridZ - 1 >= 0 && tmpCollisionMass[tmpGridZ - 1][tmpGridX] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridZ -= 1;
					arrowUpModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case MoveDown:

				if (tmpGridZ + 1 < 6 && tmpCollisionMass[tmpGridZ + 1][tmpGridX] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridZ += 1;
					arrowDownModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case Stop:
				break;
			case AttackCross:

				for (int i = 1; i < 3; i++) {

					if (tmpGridZ - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridZ + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX - i], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX + i], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				break;
			case AttackCircle:

				if (tmpGridZ - 1 >= 0) {

					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ - 1][tmpGridX], viewProjection_,
					    playerAttackMassTex_);

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridZ + 1 < 6) {

					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ + 1][tmpGridX], viewProjection_,
					    playerAttackMassTex_);

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridX - 1 >= 0) {
					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ][tmpGridX - 1], viewProjection_,
					    playerAttackMassTex_);
				}

				if (tmpGridX + 1 < 6) {
					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ][tmpGridX + 1], viewProjection_,
					    playerAttackMassTex_);
				}

				break;
			case Guard:
				break;
			case S_PlayerAttack:

				for (int i = 1; i < 3; i++) {

					if (tmpGridZ - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridZ + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX - i], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX + i], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridZ - 1 >= 0) {

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridZ + 1 < 6) {

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				break;
			default:
				break;
			}
		}

		// 現在選択しているコマンドの行動プレビュー表示
		if (player_->GetIsStart() && player_->moveCommands_.size() < kMaxCommand) {

			Command selectCommand = player_->GetSelectCommand(player_->GetSelectNum());

			switch (selectCommand) {
			case MoveLeft:

				if (tmpGridX - 1 >= 0 && tmpCollisionMass[tmpGridZ][tmpGridX - 1] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridX -= 1;
					arrowLeftModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case MoveRight:

				if (tmpGridX + 1 < 6 && tmpCollisionMass[tmpGridZ][tmpGridX + 1] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridX += 1;
					arrowRightModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case MoveUp:

				if (tmpGridZ - 1 >= 0 && tmpCollisionMass[tmpGridZ - 1][tmpGridX] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridZ -= 1;
					arrowUpModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case MoveDown:

				if (tmpGridZ + 1 < 6 && tmpCollisionMass[tmpGridZ + 1][tmpGridX] == 0) {
					tmpCollisionMass[tmpGridZ][tmpGridX] = 0;
					tmpGridZ += 1;
					arrowDownModel_->Draw(
					    worldTransformArrowMass_[tmpGridZ][tmpGridX], viewProjection_,
					    moveMassTex_);
					tmpCollisionMass[tmpGridZ][tmpGridX] = 1;
				}

				break;
			case Stop:
				break;
			case AttackCross:

				for (int i = 1; i < 3; i++) {

					if (tmpGridZ - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridZ + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX - i], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX + i], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				break;
			case AttackCircle:

				if (tmpGridZ - 1 >= 0) {

					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ - 1][tmpGridX], viewProjection_,
					    playerAttackMassTex_);

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridZ + 1 < 6) {

					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ + 1][tmpGridX], viewProjection_,
					    playerAttackMassTex_);

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridX - 1 >= 0) {
					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ][tmpGridX - 1], viewProjection_,
					    playerAttackMassTex_);
				}

				if (tmpGridX + 1 < 6) {
					groundModel_->Draw(
					    worldTransformMass_[tmpGridZ][tmpGridX + 1], viewProjection_,
					    playerAttackMassTex_);
				}

				break;
			case Guard:
				break;
			case S_PlayerAttack:

				for (int i = 1; i < 3; i++) {

					if (tmpGridZ - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridZ + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + i][tmpGridX], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX - i >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX - i], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + i < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ][tmpGridX + i], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridZ - 1 >= 0) {

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ - 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				if (tmpGridZ + 1 < 6) {

					if (tmpGridX - 1 >= 0) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX - 1], viewProjection_,
						    playerAttackMassTex_);
					}

					if (tmpGridX + 1 < 6) {
						groundModel_->Draw(
						    worldTransformMass_[tmpGridZ + 1][tmpGridX + 1], viewProjection_,
						    playerAttackMassTex_);
					}
				}

				break;
			default:
				break;
			}
		}

		for (Enemy* enemy : enemies_) {

			int tmpEnemyGridX = enemy->GetGridX();
			int tmpEnemyGridZ = enemy->GetGridZ();

			for (Command command : enemy->moveCommands_) {

				if (enemy->GetIsEffect() == false && enemy->GetIsDead() == false) {

					switch (command) {
					case MoveLeft:

						if (tmpEnemyGridX - 1 >= 0 &&
						    tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX - 1] == 0) {
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 0;
							tmpEnemyGridX -= 1;
							arrowLeftModel_->Draw(
							    worldTransformArrowMass_[tmpEnemyGridZ][tmpEnemyGridX],
							    viewProjection_, enemyMoveMassTex_);
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 1;
						}

						break;
					case MoveRight:

						if (tmpEnemyGridX + 1 < 6 &&
						    tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX + 1] == 0) {
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 0;
							tmpEnemyGridX += 1;
							arrowRightModel_->Draw(
							    worldTransformArrowMass_[tmpEnemyGridZ][tmpEnemyGridX],
							    viewProjection_, enemyMoveMassTex_);
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 1;
						}

						break;
					case MoveUp:

						if (tmpEnemyGridZ - 1 >= 0 &&
						    tmpCollisionMass[tmpEnemyGridZ - 1][tmpEnemyGridX] == 0) {
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 0;
							tmpEnemyGridZ -= 1;
							arrowUpModel_->Draw(
							    worldTransformArrowMass_[tmpEnemyGridZ][tmpEnemyGridX],
							    viewProjection_, enemyMoveMassTex_);
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 1;
						}

						break;
					case MoveDown:

						if (tmpEnemyGridZ + 1 < 6 &&
						    tmpCollisionMass[tmpEnemyGridZ + 1][tmpEnemyGridX] == 0) {
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 0;
							tmpEnemyGridZ += 1;
							arrowDownModel_->Draw(
							    worldTransformArrowMass_[tmpEnemyGridZ][tmpEnemyGridX],
							    viewProjection_, enemyMoveMassTex_);
							tmpCollisionMass[tmpEnemyGridZ][tmpEnemyGridX] = 1;
						}

						break;
					case Stop:
						break;
					case AttackCross:

						for (int i = 1; i < 3; i++) {

							if (tmpEnemyGridZ - i >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ - i][tmpEnemyGridX],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridZ + i < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ + i][tmpEnemyGridX],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX - i >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ][tmpEnemyGridX - i],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX + i < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ][tmpEnemyGridX + i],
								    viewProjection_, enemyAttackMassTex_);
							}
						}

						break;
					case AttackCircle:

						if (tmpEnemyGridZ - 1 >= 0) {

							groundModel_->Draw(
							    worldTransformMass_[tmpEnemyGridZ - 1][tmpEnemyGridX],
							    viewProjection_, enemyAttackMassTex_);

							if (tmpEnemyGridX - 1 >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ - 1][tmpEnemyGridX - 1],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX + 1 < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ - 1][tmpEnemyGridX + 1],
								    viewProjection_, enemyAttackMassTex_);
							}
						}

						if (tmpEnemyGridZ + 1 < 6) {

							groundModel_->Draw(
							    worldTransformMass_[tmpEnemyGridZ + 1][tmpEnemyGridX],
							    viewProjection_, enemyAttackMassTex_);

							if (tmpEnemyGridX - 1 >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ + 1][tmpEnemyGridX - 1],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX + 1 < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ + 1][tmpEnemyGridX + 1],
								    viewProjection_, enemyAttackMassTex_);
							}
						}

						if (tmpEnemyGridX - 1 >= 0) {
							groundModel_->Draw(
							    worldTransformMass_[tmpEnemyGridZ][tmpEnemyGridX - 1],
							    viewProjection_, enemyAttackMassTex_);
						}

						if (tmpEnemyGridX + 1 < 6) {
							groundModel_->Draw(
							    worldTransformMass_[tmpEnemyGridZ][tmpEnemyGridX + 1],
							    viewProjection_, enemyAttackMassTex_);
						}

						break;
					case Guard:
						break;
					case S_EnemyAttack:

						for (int i = 1; i < 3; i++) {

							if (tmpEnemyGridZ - i >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ - i][tmpEnemyGridX],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridZ + i < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ + i][tmpEnemyGridX],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX - i >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ][tmpEnemyGridX - i],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX + i < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ][tmpEnemyGridX + i],
								    viewProjection_, enemyAttackMassTex_);
							}
						}

						if (tmpEnemyGridZ - 1 >= 0) {

							if (tmpEnemyGridX - 1 >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ - 1][tmpEnemyGridX - 1],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX + 1 < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ - 1][tmpEnemyGridX + 1],
								    viewProjection_, enemyAttackMassTex_);
							}
						}

						if (tmpEnemyGridZ + 1 < 6) {

							if (tmpEnemyGridX - 1 >= 0) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ + 1][tmpEnemyGridX - 1],
								    viewProjection_, enemyAttackMassTex_);
							}

							if (tmpEnemyGridX + 1 < 6) {
								groundModel_->Draw(
								    worldTransformMass_[tmpEnemyGridZ + 1][tmpEnemyGridX + 1],
								    viewProjection_, enemyAttackMassTex_);
							}
						}

						break;
					default:
						break;
					}
				}
			}
		}
	}
}
