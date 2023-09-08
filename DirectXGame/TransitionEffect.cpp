#include "TransitionEffect.h"

TransitionEffect* TransitionEffect::GetInstance() {

	static TransitionEffect instance;
	return &instance;

}

void TransitionEffect::Initialize(const std::vector<uint32_t>& textures) {
	SetTextures(textures);

	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {

			if (j % 2 == 0 && i % 2 == 1 ||
				j % 2 == 1 && i % 2 == 0) {
				colorNum_ = 0;
			}
			else {
				colorNum_ = 1;
			}
			currentTex_ = textures_[colorNum_];

			transition[i][j].reset(Sprite::Create(currentTex_, {0.0f, 0.0f}));
			transition[i][j]->SetSize({0.0f, 0.0f});
			transition[i][j]->SetTextureRect({0.0f, 0.0f}, {183.0f, 180.0f});
			transition[i][j]->SetPosition({i * 183.0f, j * 180.0f});
		}
	}
	fadeOut = {0.0f, 0.0f};
	isFadeIn_ = true;
	isFadeOut_ = false;
}

void TransitionEffect::Update() {
	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {
			transition[i][j]->SetSize({horizontalSize_, verticalSize_});
			transition[i][j]->SetPosition({i * 183.0f + fadeOut.x, j * 180.0f + fadeOut.y});
			if (isFadeIn_) {
				horizontalSize_ += 0.1f;
				if (horizontalSize_ > 183.0f) {
					horizontalSize_ = 183.0f;
				}
				verticalSize_ += 0.1f;
				if (verticalSize_ > 180.0f) {
					verticalSize_ = 180.0f;
				}
				if (horizontalSize_ >= 183.0f && verticalSize_ >= 180.0f) {
					isFadeIn_ = false;
					isFadeOut_ = true;
				}
			}
			if (isFadeOut_) {
				horizontalSize_ -= 0.1f;
				fadeOut.x += 0.1f;
				if (horizontalSize_ < 0.0f) {
					horizontalSize_ = 0.0f;
					fadeOut.x = fadeOut.x;
				}
				verticalSize_ -= 0.1f;
				fadeOut.y += 0.1f;
				if (verticalSize_ < 0.0f) {
					verticalSize_ = 0.0f;
					fadeOut.y = fadeOut.y;
				}
				if (horizontalSize_ <= 0.0f && verticalSize_ <= 0.0f) {
					isFadeOut_ = false;
					isFadeIn_ = true;
				}
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

void TransitionEffect::Reset() {

	verticalSize_ = 0.0f;
	horizontalSize_ = 0.0f;

	fadeOut = {0.0f, 0.0f};

}
