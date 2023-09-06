#include "TransitionEffect.h"

void TransitionEffect::Initialize(const std::vector<uint32_t>& textures) {
	SetTextures(textures);

	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {

			currentTex_ = textures_[colorNum_];

			transition[i][j].reset(Sprite::Create(currentTex_, {0.0f, 0.0f}));
			transition[i][j]->SetSize({0.0f, 0.0f});
			transition[i][j]->SetTextureRect({0.0f, 0.0f}, {183.0f, 180.0f});
			transition[i][j]->SetPosition({i * 183.0f, j * 180.0f});

			colorNum_++;
			if (colorNum_ > 1) {
				colorNum_ = 0;
			}
		}
	}
}

void TransitionEffect::Update() {
	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {
			transition[i][j]->SetSize({horizontalSize_, verticalSize_});
			horizontalSize_ += 0.1f;
			if (horizontalSize_ > 183.0f) {
				horizontalSize_ = 183.0f;
			}
			verticalSize_ += 0.1f;
			if (verticalSize_ > 180.0f) {
				verticalSize_ = 180.0f;
			}
		}
	}
}

void TransitionEffect::Draw() {

	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {
			transition[i][j]->Draw();
		}
	}
}
