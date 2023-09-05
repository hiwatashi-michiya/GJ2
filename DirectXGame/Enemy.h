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

// コマンドの最大表示数
const int kMaxEnemyCommand = 3;

// 選択できる個数
const int kMaxEnemySelectNum = 6;

class Enemy : public MoveCommand {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sprite">スプライト</param>
	void Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

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

private:
	// 入力
	Input* input_ = nullptr;

	// 入力クールタイム
	const int kInputCoolTime = 15;

	int inputCoolTimer_ = 0;

	// プレイヤーのワールドトランスフォーム
	WorldTransform worldTransform_;

	// 速度
	Vector3 velocity_;

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
	void Move(Command command);

	// 行動コマンド画像更新
	void UpdateMoveCommandsNum();

	// スプライト
	std::unique_ptr<Sprite> commandNumSprite_[kMaxEnemyCommand];

	std::unique_ptr<Sprite> selectCommandNumSprite_[kMaxEnemySelectNum];

	std::unique_ptr<Sprite> currentNumSprite_;

	// モデル
	std::vector<Model*> models_;

	// 画像
	std::vector<uint32_t> textures_;

	// 現在セットしている画像
	uint32_t currentTex_ = 0u;
};
