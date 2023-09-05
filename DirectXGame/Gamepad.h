#pragma once

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

/// Xボックスコントローラーでやったのでコメントアウトの説明はそれ
/// ほかのコントローラーは分からないので検証必須
/// steamなどでpadの設定を行っている場合その設定のほうが優先されてしまうのでアプリを落として使用してくださいな
///
/// これの詳しい解説は http://gameprogrammingunit.web.fc2.com/xinput/index.html

class Gamepad {
public:
	// 押し込みのボタン
	enum class Button {
		UP = 0, // 十字キー上
		DOWN, // 十字キー下
		LEFT, // 十字キー左
		RIGHT, // 十字キー右
		START, // 漢字の三みたいなやつ
		BACK, // 四角が重なったようなやつ
		LEFT_THUMB, // 左スティック押し込み
		RIGHT_THUMB, // 右スティック押し込み 
		LEFT_SHOULDER, // LB
		RIGHT_SHOULDER, // RB
		A = 12, // A
		B, // B
		X, // X
		Y, // Y
	};

	// RT,LT
	enum class Triger {
		LEFT,
		RIGHT,
	};

	// スティック
	enum class Stick {
		LEFT_X,
		LEFT_Y,
		RIGHT_X,
		RIGHT_Y
	};

private:
	inline Gamepad(){}
	inline ~Gamepad(){}

public:
	// 入力関数
	static void Input();

	// 押し込みのボタンの状態を取得
	static bool getButton(const Button& type);

	// 押し込みのボタンの前フレームの状態を取得
	static bool getPreButton(const Button& type);

	// 押し込みのボタンを押した瞬間
	static bool Pushed(const Button& type);

	// 押し込みのボタンを長押し
	static bool LongPush(const Button& type);

	// 押し込みのボタンを離した瞬間
	static bool Released(const Button& type);

	// トリガーのボタンの状態を取得
	static unsigned char getTriger(const Triger& type);

	// スティックがどのくらい倒れたか
	// 左右のスティックのX,Yをそれぞれ取れる
	// 引数でどれをとるか指定
	// 戻り値の範囲は-32768～32767
	// 
	// ex: 左スティックのxをどのくらい倒したかをとる場合
	// getStick(Gamepad::Stick::LEFT_X);
	// 
	static short getStick(const Stick& type);

	// バイブレーション
	static void isVibration(const int& flag);

	// デバッグ用
	static void Draw();

private:
	static unsigned short preButton;
	static XINPUT_STATE state;
	static XINPUT_VIBRATION vibration;
};