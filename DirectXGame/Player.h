#pragma once
#include <TextureManager.h>
#include <Sprite.h>
#include "MoveCommand.h"
#include <Vector3.h>
#include <memory>
#include <Input.h>
#include <Model.h>
#include <vector>
#include <WorldTransform.h>
#include <ViewProjection.h>
#include "Collider.h"
#include "CollisionManager.h"
#include "Option.h"
#include <Audio.h>
#include "GameSpeed.h"

//コマンドの最大表示数
const int kMaxCommand = 3;

// 選択できる個数
const int kMaxSelectNum = 8;


class Player : public MoveCommand, public Collider {
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
	void Update(const ViewProjection& viewProjection,Option* option);

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
	void MoveTurn();

	bool GetIsDead() const { return isDead_; }

	bool GetIsPlayerTurn() const { return isPlayerTurn_; }

	bool GetIsSelect() const { return isSelect_; }

	void SetIsSelect(bool flag) { isSelect_ = flag; }

	void Reset();

	Vector3 GetPosition() { return worldTransform_.translation_; }

	void SetWorldPosition(Vector3 position) {
		worldTransform_.translation_ = position;
		worldTransform_.UpdateMatrix();
	}

	bool GetIsStart() { return isStart_; }

private:

	//入力
	Input* input_ = nullptr;
	//音再生
	Audio* audio_ = nullptr;

	CollisionManager* collisionManager_ = nullptr;

	GameSpeed* gameSpeed_ = nullptr;

	//入力クールタイム
	const int kInputCoolTime = 5;

	int inputCoolTimer_ = 0;

	//プレイヤーのワールドトランスフォーム
	WorldTransform worldTransform_;

	//エフェクトのワールドトランスフォーム
	WorldTransform worldTransformEffect_[8];

	//速度
	Vector3 velocity_;

	//体力
	const int kMaxLife = 100;

	int life_ = kMaxLife;
	int lifeNum_[3] = {0, 0, 0};

	//攻撃が当たったかどうか
	bool isHit_ = false;

	//死んだかどうか
	bool isDead_ = false;

	//現在の行動コマンド
	Command currentMoveCommand_ = Stop;

	//行動時間
	const int kMoveTime = 60;

	int MoveTimer_ = 0;

	//行動選択中か
	bool isSelect_ = true;

	//現在選んでいるリストの要素
	int selectNum_ = 0;

	//行動中かどうか
	bool isMove_ = false;

	//行動
	void Move(Command& command);

	//行動コマンド画像更新
	void UpdateMoveCommandsNum();

	//攻撃したかどうか
	bool isAttack_ = false;

	//ガードしたかどうか
	bool isGuard_ = false;

	//特殊攻撃発動までのカウント
	const int kMaxSpecialCount = 10;

	int specialCount_ = 10;

	//現在プレイヤーのターンかどうか
	bool isPlayerTurn_ = false;

	//インターバルタイム。行動後一瞬止める
	const int kMaxInterval = 10;

	int interval_ = 0;

	//スプライト
	std::unique_ptr<Sprite> commandNumSprite_[kMaxCommand];

	std::unique_ptr<Sprite> selectCommandNumSprite_[kMaxSelectNum];

	std::unique_ptr<Sprite> currentNumSprite_;

	std::unique_ptr<Sprite> findUI_[kMaxSelectNum];
	
	std::unique_ptr<Sprite> hpFrameSprite_;
	std::unique_ptr<Sprite> hpSprite_[3];

	std::unique_ptr<Sprite> nextUISprite_;

	// スプライトの位置をセット
	void SetCommandSprite(const ViewProjection& viewProjection);

	//モデル
	std::vector<Model*> models_;

	//画像
	std::vector<uint32_t> textures_;

	//SE
	std::vector<uint32_t> sounds_;
	uint32_t crossAttackSE_ = 0u;
	uint32_t upMoveSE_ = 0u;
	uint32_t clickSE_ = 0u;
	uint32_t selectSE_ = 0u;
	uint32_t cancelSE_ = 0u;
	uint32_t fallSE_ = 0u;

	//現在セットしている画像
	uint32_t currentTex_ = 0u;

	// 行動UIの表示
	bool isFindUI_[kMaxSelectNum] = {};
	
	uint32_t findUITexture_;

	float moveAngle_;

	//スタート演出
	bool isStart_ = false;

	int startCount_ = 60;

};
