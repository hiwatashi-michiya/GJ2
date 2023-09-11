#pragma once
#include"Input.h"
#include"WorldTransform.h"
#include"Sprite.h"
#include "TextureManager.h"
#include "Model.h"
#include "DirectXCommon.h"

enum ActCode {
	ACT = 1,
	SPECIAL ,
	CANCEL,
	MENU,
	L_SELECT,
	R_SELECT,
	U_SELECT,
	D_SELECT,
	UI_SELECT,
	DASH,

	END = 64,
};

class Option {
public: 
	
	Option();
	~Option();
	void Initialize();
	void Update();
	void Draw();

	bool GetActionTrigger(ActCode act);
	bool GetActionLongPush(ActCode act);
	void CursorUpdate();
	Vector2 GetCursorPos();
	Vector2 GetCursorRad();

private:

	// カーソル
	WorldTransform m_cursorWorldTransform;
	Sprite* m_cursorSprite = nullptr;
	Vector3 m_cursorPos = {60, 120, 0};
	Vector3 m_cursorVel = {6, 6, 0};

	uint32_t cursorTextureHandle = 0u;

public:

	// BGM
	float m_bgmVol;

	// SE
	float m_seVol;


	// 設定キー
	int m_InputButton[ActCode::END];

};
