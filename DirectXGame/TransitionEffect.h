#pragma once
#include <Sprite.h>
#include <TextureManager.h>
#include <Vector3.h>
#include <vector>

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

private:
	int moveTime_;

	// スプライト
	std::unique_ptr<Sprite> transition[kHorizontalDivisionMax][kVerticalDivisionMax];

	// 画像
	std::vector<uint32_t> textures_;

	// 現在セットしている画像
	uint32_t currentTex_ = 0u;
};
