#pragma once
#include "MoveCommand.h"
#include <Input.h>
#include <Model.h>
#include <Sprite.h>
#include <TextureManager.h>
#include <Vector3.h>
#include <ViewProjection.h>
#include <WorldTransform.h>
#include <memory>
#include <vector>
#include "Collider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "GameSpeed.h"
#include <Audio.h>

// コマンドの最大表示数
const int kMaxEnemyCommand = 3;

// 選択できる個数
const int kMaxEnemySelectNum = 6;

enum MovePattern {
	//基本行動
	E_Move,
	E_Attack,
	E_Guard,
	E_Rush,
	E_Stan,
	//特別行動
	E_Special,

};

class Enemy : public MoveCommand, public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sprite">スプライト</param>
	void Initialize(
	    const std::vector<Model*>& models, const std::vector<uint32_t>& textures,
	    const std::vector<uint32_t>& sounds);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection& viewProjection, Option* option);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// モデル配列のセット
	/// </summary>
	/// <param name="models">モデル配列</param>
	void SetModels(const std::vector<Model*>& models) { models_ = models; }

	/// <summary>
	/// 画像配列のセット
	/// </summary>
	/// <param name="textures">画像配列</param>
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }

	/// <summary>
	/// 音声配列のセット
	/// </summary>
	/// <param name="sounds">音声配列</param>
	void SetSounds(const std::vector<uint32_t>& sounds) { sounds_ = sounds; }

	/// <summary>
	/// 行動ターン
	/// </summary>
	void MoveTurn(const ViewProjection& viewProjection);

	bool GetIsDead() const { return isDead_; }

	bool GetIsEnemyTurn() const { return isEnemyTurn_; }

	void SetPosition(int x, int z);

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsSelect() const { return isSelect_; }

	void SetIsSelect(bool flag) { isSelect_ = flag; }

	void Reset();

private:
	// 入力
	Input* input_ = nullptr;
	// 音再生
	Audio* audio_ = nullptr;

	CollisionManager* collisionManager_ = nullptr;

	GameSpeed* gameSpeed_ = nullptr;

	Player* player_ = nullptr;

	// 入力クールタイム
	const int kInputCoolTime = 15;

	int inputCoolTimer_ = 0;

	// プレイヤーのワールドトランスフォーム
	WorldTransform worldTransform_;

	// 速度
	Vector3 velocity_;

	// 体力
	const int kMaxLife = 100;

	int life_ = kMaxLife;

	// 攻撃が当たったかどうか
	bool isHit_ = false;

	// 死んだかどうか
	bool isDead_ = false;

	// 現在の行動コマンド
	Command currentMoveCommand_ = Stop;

	// 行動時間
	const int kMoveTime = 60;

	int MoveTimer_ = 0;

	// 行動選択中か
	bool isSelect_ = true;

	// 現在選んでいるリストの要素
	int selectNum_ = 0;

	// 行動中かどうか
	bool isMove_ = false;

	// 行動
	void Move(Command& command);

	// 攻撃したかどうか
	bool isAttack_ = false;

	// ガードしたかどうか
	bool isGuard_ = false;

	// 行動コマンド画像更新
	void UpdateMoveCommandsNum();

	//エネミーの行動パターン
	MovePattern movePattern_ = E_Move;

	//攻撃ラッシュに入るまでのカウント
	int32_t rushCount_ = 0;

	//形態変化フラグ
	bool isFormChange_ = false;

	//行動パターン更新
	void SetEnemyMovePattern();

	// 現在プレイヤーのターンかどうか
	bool isEnemyTurn_ = false;

	// インターバルタイム。行動後一瞬止める
	const int kMaxInterval = 10;

	int interval_ = 0;

	// スプライト
	std::unique_ptr<Sprite> commandNumSprite_[kMaxEnemyCommand];

	std::unique_ptr<Sprite> selectCommandNumSprite_[kMaxEnemySelectNum];

	std::unique_ptr<Sprite> currentNumSprite_;

	//スプライトの位置をセット
	void SetCommandSprite(const ViewProjection& viewProjection);

	// モデル
	std::vector<Model*> models_;

	// 画像
	std::vector<uint32_t> textures_;

	// SE
	std::vector<uint32_t> sounds_;

	// 現在セットしている画像
	uint32_t currentTex_ = 0u;
};
