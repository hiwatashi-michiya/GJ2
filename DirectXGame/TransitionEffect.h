#pragma once
#include <Sprite.h>
#include <TextureManager.h>
#include <Vector3.h>
#include "scene.h"

const int kHorizontalDivisionMax = 7;
const int kVerticalDivisionMax = 4;

class TransitionEffect {
public:

	static TransitionEffect* GetInstance();

	void Initialize(const std::vector<uint32_t>& textures);
	void Update();
	void Draw();

	/// <summary>
	/// 画像配列のセット
	/// </summary>
	/// <param name="textures">画像配列</param>
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }

	// シーン遷移ゲッター関数
	Scene GetCurrentScene() { return scene_; }

	// 遷移先シーンゲッター関数
	Scene GetNextScene() { return nextScene_; }

	//フェードインアウトゲッター関数
	bool GetFadeIn() { return isFadeIn_; }
	bool GetFadeOut() { return isFadeOut_; }

	/// <summary>
	/// ポジションリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 遷移先のシーンに切り替える
	/// </summary>
	void ChangeScene() { scene_ = nextScene_; }

	/// <summary>
	/// 遷移先のシーンを設定する
	/// </summary>
	/// <param name="setScene">セットするシーン</param>
	void SetNextScene(Scene setScene) { nextScene_ = setScene; }

	bool GetIsChangeScene() { return isChangeScene_; }

	void SetIsChangeScene(bool flag) { isChangeScene_ = flag; }

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

	//現在のシーン
	Scene scene_ = TITLE;

	//遷移先のシーン
	Scene nextScene_ = TITLE;

	//遷移中かどうか
	bool isChangeScene_ = false;

private:

	TransitionEffect() = default;
	~TransitionEffect() = default;
	TransitionEffect(const TransitionEffect&) = delete;
	const TransitionEffect& operator=(const TransitionEffect&) = delete;

};
