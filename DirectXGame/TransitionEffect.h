#pragma once
#include <Sprite.h>
#include <TextureManager.h>
#include <Vector3.h>
#include "scene.h"

const int kHorizontalDivisionMax = 7;
const int kVerticalDivisionMax = 4;

class TransitionEffect {
public:
	void Initialize(const std::vector<uint32_t>& textures);
	void Update();
	void Draw();

	/// <summary>
	/// 画像配列のセット
	/// </summary>
	/// <param name="textures">画像配列</param>
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }

	// シーン遷移ゲッター関数
	int32_t GetCurrentScene() { return scene; }

	//フェードインアウトゲッター関数
	bool GetFadeIn() { return isFadeIn_; }
	bool GetFadeOut() { return isFadeOut_; }

private:
	int colorNum_ = 0;
	float verticalSize_ = 0.0f;
	float horizontalSize_ = 0.0f;

	bool isFadeIn_ = false;
	bool isFadeOut_ = false;
	Vector2 fadeOut = {0.0f, 0.0f};

	// スプライト
	std::unique_ptr<Sprite> transition[kHorizontalDivisionMax][kVerticalDivisionMax];

	// 画像
	std::vector<uint32_t> textures_;

	// 現在セットしている画像
	uint32_t currentTex_ = 0u;

	int32_t scene = TITLE;
};