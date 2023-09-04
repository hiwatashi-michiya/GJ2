#pragma once
#include"Input.h"

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
	void Update();
	void Draw();

	bool GetActionTrigger(ActCode act);

private:

	// BGM
	float m_bgmVol;

	// SE
	float m_seVol;

	public:

	// 設定キー
	int m_InputButton[ActCode::END];

};
