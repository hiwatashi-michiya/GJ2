#include "Option.h"
#include "Gamepad.h"
#include "ImGuiManager.h"
#include <assert.h>

Option::Option() {
	// 初期キー設定
	m_InputButton[ActCode::MOVE] = Gamepad::getStick(Gamepad::Stick::LEFT_X);
	m_InputButton[ActCode::JUMP] = (int)Gamepad::Button::Y;
	m_InputButton[ActCode::DASH] = (int)Gamepad::Button::RIGHT_SHOULDER;
	m_InputButton[ActCode::ACT] = (int)Gamepad::Button::B;
	m_InputButton[ActCode::MENU] = (int)Gamepad::Button::START;
	m_InputButton[ActCode::CANCEL] = (int)Gamepad::Button::A;

	this->Initialize();
}

Option::~Option() {}

void Option::Initialize() 
{
	dxCommon_ = DirectXCommon::GetInstance();

	cursorTextureHandle = TextureManager::Load("cursorImage.png");
	assert(cursorTextureHandle);
	
	m_cursorSprite = Sprite::Create(
	    cursorTextureHandle, {m_cursorPos.x, m_cursorPos.y}, 
		{1.0f, 1.0f, 1.0f, 1.0f},{0.5f, 0.5f});
	m_cursorSprite->SetSize({64.0f, 64.0f});


}

void Option::Update() {
	
	// ゲームパッドのインスタンスを取得
	Gamepad::Input();

	// カーソルの更新処理
	CursorUpdate();


	ImGui::Begin("Action");
	ImGui::Text("JUMP = %d", m_InputButton[ActCode::JUMP]);
	ImGui::Text("DASH = %d", m_InputButton[ActCode::DASH]);
	ImGui::Text("ACT = %d", m_InputButton[ActCode::ACT]);
	ImGui::Text("MENU = %d", m_InputButton[ActCode::MENU]);
	ImGui::Text("CANCEL = %d", m_InputButton[ActCode::CANCEL]);
	ImGui::End();

	ImGui::Begin("Option");
	ImGui::Text("Cursor_X = %d", m_cursorPos.x);
	ImGui::Text("Cursor_Y = %d", m_cursorPos.y);
	ImGui::DragInt("Jump", &m_InputButton[ActCode::JUMP], 1.0f, 0, 15);
	ImGui::End();
}

void Option::Draw() 
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

		m_cursorSprite->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}

bool Option::GetActionTrigger(ActCode act) {

	switch (act) {

	case JUMP:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::JUMP] == i) {
				switch (i) {
				case 0:
					if (Gamepad::Pushed(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::Pushed(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::Pushed(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::Pushed(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::Pushed(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::Pushed(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::Pushed(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::Pushed(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::Pushed(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case DASH:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::DASH] == i) {
				switch (i) {
				case 0:
					if (Gamepad::Pushed(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::Pushed(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::Pushed(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::Pushed(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::Pushed(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::Pushed(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::Pushed(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::Pushed(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::Pushed(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case ACT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::ACT] == i) {
				switch (i) {
				case 0:
					if (Gamepad::Pushed(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::Pushed(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::Pushed(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::Pushed(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::Pushed(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::Pushed(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::Pushed(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::Pushed(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::Pushed(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case MENU:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::MENU] == i) {
				switch (i) {
				case 0:
					if (Gamepad::Pushed(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::Pushed(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::Pushed(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::Pushed(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::Pushed(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::Pushed(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::Pushed(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::Pushed(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::Pushed(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case CANCEL:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::CANCEL] == i) {
				switch (i) {
				case 0:
					if (Gamepad::Pushed(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::Pushed(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::Pushed(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::Pushed(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::Pushed(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::Pushed(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::Pushed(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::Pushed(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::Pushed(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::Pushed(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::Pushed(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case END:

		break;
	default:

		break;
	}

	return false;
}

void Option::CursorUpdate() {

	if (Gamepad::getStick(Gamepad::Stick::LEFT_X) >= 3000) {
		m_cursorPos.x += m_cursorVel.x;
	}
	if (Gamepad::getStick(Gamepad::Stick::LEFT_X) <= -3000) {
		m_cursorPos.x -= m_cursorVel.x;
	}
	if (Gamepad::getStick(Gamepad::Stick::LEFT_Y) >= 3000) {
		m_cursorPos.y += m_cursorVel.x;
	}
	if (Gamepad::getStick(Gamepad::Stick::LEFT_Y) <= -3000) {
		m_cursorPos.y -= m_cursorVel.x;
	}

	m_cursorSprite->SetPosition({m_cursorPos.x, m_cursorPos.y});
}
