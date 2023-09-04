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

class Player : public MoveCommand {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sprite">スプライト</param>
	void Initialize(Sprite* sprite);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// モデル配列のセット
	/// </summary>
	/// <param name="models">モデル配列</param>
	void SetModels(const std::vector<Model*>& models) { models_ = models; }

	/// <summary>
	/// 画像配列のセット
	/// </summary>
	/// <param name="textures">画像配列</param>
	void SetTextures(const std::vector<uint32_t>& textures) {

		textures_ = textures;
		currentTex_ = textures_[0];

	}

private:

	//入力
	Input* input_ = nullptr;

	//プレイヤーのワールドトランスフォーム
	WorldTransform worldTransform_;

	//描画位置(中央)
	Vector3 position_;

	//速度
	Vector3 velocity_;

	//現在の行動コマンド
	Command currentMoveCommand_ = Stop;

	//行動時間
	const int kMoveTime = 60;

	int MoveTimer_ = 0;

	//行動中かどうか
	bool isMove_ = false;

	//行動
	void Move(Command command);

	//スプライト
	Sprite* playerSprite_;

	//モデル
	std::vector<Model*> models_;

	//画像
	std::vector<uint32_t> textures_;

	//現在セットしている画像
	uint32_t currentTex_ = 0u;

	//画像位置設定。ポジションの変更後に使用
	void SetSpritePosition() {
		playerSprite_->SetPosition(
		    {position_.x - kTextureWidth / 2.0f, position_.y - kTextureHeight / 2.0f});
	}

	//画像横幅
	const int kTextureWidth = 32;
	//画像縦幅
	const int kTextureHeight = 32;

};
