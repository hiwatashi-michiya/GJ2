#include "TransitionEffect.h"

void TransitionEffect::Initialize(const std::vector<uint32_t>& textures) { 
	SetTextures(textures);

	currentTex_ = textures_[0];
	moveTime_ = 0;

	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {
			transition[i][j].reset(Sprite::Create(textures_[4], {0.0f, 0.0f}));
			transition[i][j]->SetSize({64.0f, 64.0f});
			transition[i][j]->SetTextureRect(
			    {
			        0.0f,
			        0.0f,
			    },
			    {64.0f, 64.0f});
			transition[i][j]->SetPosition(
			    {i * 64.0f, j * 64.0f});
		}
	}
}

void TransitionEffect::Update() {

}

void TransitionEffect::Draw() {

	for (int i = 0; i < kHorizontalDivisionMax; i++) {
		for (int j = 0; j < kVerticalDivisionMax; j++) {
			transition[i][j]->Draw();
		}
	}
}
