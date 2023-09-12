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

class Option final{
private: 
	
	Option();
	~Option();

public:

	// コピーや代入演算子を無効化
	Option(const Option& option) = delete;
	Option& operator=(const Option& option) = delete;

	static Option* GetInstance() { 
		static Option instance;
		return &instance;
	}

	void Initialize();
	void Update();
	void Draw();

	bool GetActionTrigger(ActCode act);
	bool GetActionLongPush(ActCode act);
	void CursorUpdate();
	Vector2 GetCursorPos();
	Vector2 GetCursorRad();

	bool GetMenuOverlay() { return isMenuOverlay_; };

private:

	// カーソル
	WorldTransform m_cursorWorldTransform;
	Vector3 m_cursorPos = {60, 120, 0};
	Vector3 m_cursorVel = {6, 6, 0};
	
	// スプライト
	Sprite* m_cursorSprite_ = nullptr;
	Sprite* m_menuSprite = nullptr;
	// 画像
	uint32_t cursorTextureHandle_ = 0u;
	uint32_t menuTextureHandal_ = 0u;


	bool isMenuOverlay_;

public:

	// BGM
	float m_bgmVol;

	// SE
	float m_seVol;


	// 設定キー
	int m_InputButton[ActCode::END];

};
