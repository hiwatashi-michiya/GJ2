#pragma once
#include"Input.h"
#include"WorldTransform.h"
#include"Sprite.h"
#include "TextureManager.h"
#include "Model.h"
#include "DirectXCommon.h"

enum ActCode {
	MOVE,
	JUMP,
	DASH,
	ACT,
	MENU,
	CANCEL,

	END = 64,
};

class Option {
public: 
	
	Option();
	~Option();
	void Initialize();
	void Update(const ViewProjection& viewprojection);
	void Draw();

	bool GetActionTrigger(ActCode act);
	void CursorUpdate();

private:

	// カーソル
	WorldTransform m_cursorWorldTransform;
	Sprite* m_cursorSprite = nullptr;
	Vector3 m_cursorPos = {640, 320, 0};
	Vector3 m_cursorVel = {6, 6, 0};

	uint32_t cursorTextureHandle = 0u;

	// BGM
	float m_bgmVol;

	// SE
	float m_seVol;

	public:

	// 設定キー
	int m_InputButton[ActCode::END];

};
