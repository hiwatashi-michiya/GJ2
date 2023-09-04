#pragma once
///
/// 入門用関数群
///
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// 色指定サンプル
#define WHITE 0xFFFFFFFF // 白
#define BLACK 0x000000FF // 黒
#define RED   0xFF0000FF // 赤
#define GREEN 0x00FF00FF // 緑
#define BLUE  0x0000FFFF // 青

// ブレンドモード
enum BlendMode {
	kBlendModeNone,   //!< ブレンドなし
	kBlendModeNormal, //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	kBlendModeAdd,    //!< 加算。Src * SrcA + Dest * 1
	kBlendModeSubtract, //!< 減算。Dest * 1 - Src * SrcA
	kBlendModeMultily,  //!< 乗算。Src * 0 + Dest * Src
	kBlendModeScreen,   //!< スクリーン。Src * (1 - Dest) + Dest * 1

	// 利用してはいけない
	kCountOfBlendMode,
};

// フィルモード
enum FillMode {
	kFillModeSolid,     //!< ソリッド
	kFillModeWireFrame, //!< ワイヤーフレーム
};

// ウィンドウモード
enum WindowMode {
	kWindowed,   //!< ウィンドウ
	kFullscreen, //!< フルスクリーン
};

// ゲームパッドボタン
enum PadButton {
	kPadButton0,  //!< XInputの場合、十字キー上
	kPadButton1,  //!< XInputの場合、十字キー下
	kPadButton2,  //!< XInputの場合、十字キー左
	kPadButton3,  //!< XInputの場合、十字キー右
	kPadButton4,  //!< XInputの場合、START
	kPadButton5,  //!< XInputの場合、BACK
	kPadButton6,  //!< XInputの場合、L3
	kPadButton7,  //!< XInputの場合、R3
	kPadButton8,  //!< XInputの場合、L1
	kPadButton9,  //!< XInputの場合、R1
	kPadButton10, //!< XInputの場合、L2
	kPadButton11, //!< XInputの場合、R2
	kPadButton12,
	kPadButton13,
	kPadButton14,
	kPadButton15,
	kPadButton16,
	kPadButton17,
	kPadButton18,
	kPadButton19,
};

class Novice {

public:
	/// <summary>
	/// システム全体の初期化
	/// <param name="title">タイトルバー</param>
	/// <param name="width">ウィンドウ（クライアント領域）の幅</param>
	/// <param name="height">ウィンドウ（クライアント領域）の高さ</param>
	/// </summary>
	static void Initialize(const char* title, int width = 1280, int height = 720);

	/// <summary>
	/// システム全体の終了
	/// </summary>
	static void Finalize();

	/// <summary>
	/// 四角形を描画する
	/// </summary>
	/// <param name="x">描画する四角形の左上座標 X</param>
	/// <param name="y">描画する四角形の左上座標 Y</param>
	/// <param name="w">描画する四角形の横幅</param>
	/// <param name="h">描画する四角形の縦幅</param>
	/// <param name="angle">描画する四角形の回転角</param>
	/// <param name="color">描画する四角形の色</param>
	/// <param name="fillMode">塗りつぶしモード</param>
	static void
	    DrawBox(int x, int y, int w, int h, float angle, unsigned int color, FillMode fillMode);

	/// <summary>
	/// 三角形を描画する
	/// </summary>
	/// <param name="x1">描画する三角形の頂点1 X</param>
	/// <param name="y1">描画する三角形の頂点1 Y</param>
	/// <param name="x2">描画する三角形の頂点2 X</param>
	/// <param name="y2">描画する三角形の頂点2 Y</param>
	/// <param name="x3">描画する三角形の頂点3 X</param>
	/// <param name="y3">描画する三角形の頂点3 Y</param>
	/// <param name="color">描画する三角形の色</param>
	/// <param name="fillMode">塗りつぶしモード</param>
	static void DrawTriangle(
	    int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color, FillMode fillMode);

	/// <summary>
	/// 線を描画する
	/// </summary>
	/// <param name="x1">描画する線の始点座標 X</param>
	/// <param name="y1">描画する線の始点座標 Y</param>
	/// <param name="x2">描画する線の終点座標 X</param>
	/// <param name="y2">描画する線の終点座標 Y</param>
	/// <param name="color">描画する三角形の色</param>
	static void DrawLine(int x1, int y1, int x2, int y2, unsigned int color);

	/// <summary>
	/// 楕円を描画する
	/// </summary>
	/// <param name="x">楕円の中心座標 X</param>
	/// <param name="y">楕円の中心座標 Y</param>
	/// <param name="radiusX">楕円の半径 X</param>
	/// <param name="radiusY">楕円の半径 Y</param>
	/// <param name="angle">楕円の回転角(rad)</param>
	/// <param name="color">楕円の色</param>
	static void DrawEllipse(
	    int x, int y, int radiusX, int radiusY, float angle, unsigned int color, FillMode fillMode);

	/// <summary>
	/// 画像ファイルを読み込む
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャのハンドル</returns>
	static int LoadTexture(const char* fileName);

	/// <summary>
	/// 画像ファイルの読み込みを解除する
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	static void UnloadTexture(int textureHandle);

	/// <summary>
	/// スプライトを描画する
	/// </summary>
	/// <param name="x">描画するスプライトの左上座標 X</param>
	/// <param name="y">描画するスプライトの左上座標 Y</param>
	/// <param name="textureHandle">テクスチャのハンドル</param>
	/// <param name="scale">描画するスプライトの倍率 X</param>
	/// <param name="scale">描画するスプライトの倍率 Y</param>
	/// <param name="angle">描画するスプライトの回転角</param>
	/// <param name="color">描画するスプライトの色</param>
	static void DrawSprite(
	    int x, int y, int textureHandle, float scaleX, float scaleY, float angle,
	    unsigned int color);

	/// <summary>
	/// スプライトの指定矩形部分のみを描画を描画する
	/// </summary>
	/// <param name="destX">描画するスプライトの左上座標 X</param>
	/// <param name="destY">描画するスプライトの左上座標 X</param>
	/// <param name="srcX">画像上の描画したい範囲左上座標 X</param>
	/// <param name="srcY">画像上の描画したい範囲左上座標 Y</param>
	/// <param name="srcW">画像上の描画したい範囲横幅</param>
	/// <param name="srcH">画像上の描画したい範囲縦幅</param>
	/// <param name="textureHandle">テクスチャのハンドル</param>
	/// <param name="scale">描画するスプライトの倍率 X</param>
	/// <param name="scale">描画するスプライトの倍率 Y</param>
	/// <param name="angle">描画するスプライトの回転角</param>
	/// <param name="color">描画するスプライトの色</param>
	static void DrawSpriteRect(
	    int destX, int destY, int srcX, int srcY, int srcW, int srcH, int textureHandle,
	    float scaleX, float scaleY, float angle, unsigned int color);

	/// <summary>
	/// 四角形描画
	/// </summary>
	/// <param name="x1">描画する四角形の左上座標 X</param>
	/// <param name="y1">描画する四角形の左上座標 Y</param>
	/// <param name="x2">描画する四角形の右上座標 X</param>
	/// <param name="y2">描画する四角形の右上座標 Y</param>
	/// <param name="x3">描画する四角形の左下座標 X</param>
	/// <param name="y3">描画する四角形の左下座標 Y</param>
	/// <param name="x4">描画する四角形の右下座標 X</param>
	/// <param name="y4">描画する四角形の右下座標 Y</param>
	/// <param name="srcX">画像上の描画したい範囲左上座標 X</param>
	/// <param name="srcY">画像上の描画したい範囲左上座標 Y</param>
	/// <param name="srcW">画像上の描画したい範囲横幅</param>
	/// <param name="srcH">画像上の描画したい範囲縦幅</param>
	/// <param name="textureHandle">テクスチャのハンドル</param>
	/// <param name="color">描画するスプライトの色</param>
	static void DrawQuad(
	    int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int srcX, int srcY,
	    int srcW, int srcH, int textureHandle, unsigned int color);

	/// <summary>
	/// 音ファイルを読み込む
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>オーディオハンドル</returns>
	static int LoadAudio(const char* fileName);

	/// <summary>
	/// 音データを再生する
	/// </summary>
	/// <param name="soundHandle">オーディオハンドル</param>
	/// <param name="loopFlag">1: ループする 0:ループしない</param>
	/// <param name="volume">ボリューム
	/// 0で無音、1がデフォルト音量。あまり大きくしすぎると音割れする</param>
	/// <returns>プレイハンドル</returns>
	static int PlayAudio(int soundHandle, int loopFlag, float volume);

	/// <summary>
	/// 再生中の音を止める
	/// </summary>
	/// <param name="playHandle">プレイハンドル</param>
	static void StopAudio(int playHandle);

	/// <summary>
	/// 再生中の音を一時停止する
	/// </summary>
	/// <param name="playHandle">プレイハンドル</param>
	static void PauseAudio(int playHandle);

	/// <summary>
	/// 一時停止中の音を再開する
	/// </summary>
	/// <param name="playHandle">プレイハンドル</param>
	static void ResumeAudio(int playHandle);

	/// <summary>
	/// 音量設定
	/// </summary>
	/// <param name="playHandle">プレイハンドル</param>
	/// <param name="volume">ボリューム
	/// 0で無音、1がデフォルト音量。あまり大きくしすぎると音割れする</param>
	static void SetAudioVolume(int playHandle, float volume);

	/// <summary>
	/// 音が再生中かどうか。StopAudioを呼んでいなくても、最後まで再生してループしていない場合はfalseが変えるようになります。
	/// </summary>
	/// <param name="playHandle">プレイハンドル</param>
	/// <returns>音を再生中かどうか。0:再生中ではない 1:再生中</returns>
	static int IsPlayingAudio(int playHandle);

	/// <summary>
	/// 特定キーの入力状態を得る
	/// </summary>
	/// <param name="keyCode">入力状態を取得するキーコード( DIK_0 等)</param>
	/// <returns>1: 押されている 0: 押されていない</returns>
	static int CheckHitKey(int keyCode);

	/// <summary>
	/// キーボードのすべてのキーの入力状態を取得する
	/// </summary>
	/// <param name="keyStateBuf">すべてのキーの押下状態を格納するバッファのポインタ</param>
	/// <returns></returns>
	static void GetHitKeyStateAll(char* keyStateBuf);

	/// <summary>
	/// 特定マウスボタンが押されているかを得る
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>1: 押されている 0: 押されていない</returns>
	static int IsPressMouse(int buttonNumber);

	/// <summary>
	/// 特定マウスボタンがトリガーされたかを得る
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>1: トリガーされた 0: トリガーされていない</returns>
	static int IsTriggerMouse(int buttonNumber);

	/// <summary>
	/// マウスの位置を取得する
	/// </summary>
	/// <param name="positionX">x位置を格納</param>
	/// <param name="positionY">y位置を格納</param>
	/// <returns>1: 成功 0: 失敗</returns>
	static int GetMousePosition(int* positionX, int* positionY);

	/// <summary>
	/// マウスカーソルの表示・非表示を設定する
	/// </summary>
	/// <param name="visibility">0:非表示,1:表示</param>
	static void SetMouseCursorVisibility(int visibility);

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
	static int GetWheel();

	/// <summary>
	/// 描画する際のブレンドモードを設定する。初期値はkBlendModeNormal
	/// <param name="blendMode">ブレンドモード</param>
	/// </summary>
	static void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// ウィンドウアイコンを変更する
	/// <param name="fileName">ファイル名</param>
	/// </summary>
	static void SetIcon(const char* fileName);

	/// <summary>
	/// ウィンドウモードを設定する
	/// <param name="windowMode">ウィンドウモード</param>
	/// </summary>
	static void SetWindowMode(WindowMode windowMode);

	/// <summary>
	/// ウィンドウサイズ変更を許可するかどうか。変更を許可しないに変えた場合、変えた際のウィンドウサイズで固定されます。
	/// <param name="allow">0:許可しない 1:許可する</param>
	/// </summary>
	static void AllowWindowSizeChanged(int allow);

	/// ジョイスティックの数を取得する
	/// </summary>
	/// <returns>ジョイスティックの数</returns>
	static int GetNumberOfJoysticks();

	/// 現在のジョイスティック状態を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="out">現在のジョイスティック状態</param>
	/// <returns>正しく取得できたか</returns>
	static int GetJoystickState(int stickNo, DIJOYSTATE2* out);

	/// <summary>
	/// ジョイスティックの指定ボタンがトリガーされたかを得る
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="buttonNumber">ジョイスティックのボタン番号</param>
	/// <returns>1: トリガーされた 0: トリガーされていない</returns>
	static int IsTriggerButton(int stickNo, PadButton button);

	/// <summary>
	/// ジョイスティックの指定ボタンがトリガーされたかを得る
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="button">ジョイスティックのボタン番号</param>
	/// <returns>1: トリガーされた 0: トリガーされていない</returns>
	static int IsPressButton(int stickNo, PadButton button);

	/// <summary>
	/// 左アナログスティックの値を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="x">アナログスティックの左右値 0~65535(0xFFFF) 0x8000が真ん中</param>
	/// <param name="y">アナログスティックの上下値 0~65535(0xFFFF) 0x8000が真ん中</param>
	/// <returns>1: 正しく取得できた 0: 正しく取得できなかった</returns>
	static int GetAnalogInputLeft(int stickNo, int* x, int* y);

	/// <summary>
	/// 左アナログスティックの値を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="x">アナログスティックの左右値 0~65535(0xFFFF) 0x8000が真ん中</param>
	/// <param name="y">アナログスティックの上下値 0~65535(0xFFFF) 0x8000が真ん中</param>
	/// <returns>1: 正しく取得できた 0: 正しく取得できなかった</returns>
	static int GetAnalogInputRight(int stickNo, int* x, int* y);

	/// <summary>
	/// デッドゾーンを設定する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="deadZoneL">デッドゾーン左スティック 0~32768</param>
	/// <param name="deadZoneR">デッドゾーン右スティック 0~32768</param>
	/// <returns>正しく取得できたか</returns>
	static void SetJoystickDeadZone(int stickNo, int deadZoneL, int deadZoneR);

	/// <summary>
	/// スクリーンにデバッグ用文字列を表示する
	/// </summary>
	/// <param name="x">表示するデバッグ用文字列の左上座標 X</param>
	/// <param name="y">表示するデバッグ用文字列の左上座標 Y</param>
	/// <param name="format">書式付き文字列(printfと同等)</param>
	static void ScreenPrintf(int x, int y, const char* format, ...);

	/// <summary>
	/// 出力ウィンドウにデバッグ用文字列を表示する
	/// </summary>
	/// <param name="format">書式付き文字列(printfと同等)</param>
	static void ConsolePrintf(const char* format, ...);

	/// <summary>
	/// Windowsのメッセージを処理する
	/// </summary>
	/// <returns>1: ゲーム終了 0: ゲーム継続</returns>
	static int ProcessMessage();

	/// <summary>
	/// フレーム開始処理。ブレンドモードはkBlendNormalに設定されます。
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// フレーム終了処理
	/// </summary>
	static void EndFrame();
};
