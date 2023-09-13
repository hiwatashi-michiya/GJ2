#include "Option.h"
#include "Gamepad.h"
#include "ImGuiManager.h"
#include <assert.h>

Option::Option() {
	// 初期キー設定
	m_InputButton[ActCode::SPECIAL] = (int)Gamepad::Button::RIGHT_SHOULDER;
	m_InputButton[ActCode::ACT] = (int)Gamepad::Button::A;
	m_InputButton[ActCode::CANCEL] = (int)Gamepad::Button::B;
	m_InputButton[ActCode::MENU] = (int)Gamepad::Button::START;
	m_InputButton[ActCode::L_SELECT] = (int(Gamepad::Button::LEFT));
	m_InputButton[ActCode::R_SELECT] = (int(Gamepad::Button::RIGHT));
	m_InputButton[ActCode::U_SELECT] = (int(Gamepad::Button::UP));
	m_InputButton[ActCode::D_SELECT] = (int(Gamepad::Button::DOWN));
	m_InputButton[ActCode::UI_SELECT] = (int(Gamepad::Button::X));
	m_InputButton[ActCode::DASH] = (int)Gamepad::Button::RIGHT_THUMB;

	m_bgmVol = 0.5f;
	m_seVol = 0.5f;
	isMenuOverlay_ = false;

	this->Initialize();
}

Option::~Option() {}

void Option::Initialize() {
	input_ = Input::GetInstance();

	// 画像
	menuTextureHandal_ = TextureManager::Load("UI/optionOverLay.png");
	cursorTextureHandle_ = TextureManager::Load("cursorImage.png");
	numberUITex_ = TextureManager::Load("UI/numberplateYelow.png");
	leftUITex_ = TextureManager::Load("UI/LEFT.png");
	rightUITex_ = TextureManager::Load("UI/RIGHT.png");

	m_menuSprite.reset(
		Sprite::Create(menuTextureHandal_, { 0, 0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
	m_menuSprite->SetTextureRect({ 0.0f, 0.0f }, { 2560.0f, 1440.0f });
	m_menuSprite->SetSize({ 1280.0f, 720.0f });

	m_cursorSprite_.reset(Sprite::Create(
		cursorTextureHandle_, { m_cursorPos.x, m_cursorPos.y }, { 1.0f, 1.0f, 0.0f, 1.0f },
		{ 0.5f, 0.5f }));
	m_cursorSprite_->SetSize({ 32.0f, 32.0f });

	for (int i = 0; i < 2; i++) {

		m_leftSelectUI[i].reset(Sprite::Create(leftUITex_, { 100.0f, (i + 1) * 300.0f - 110.0f }));
		m_leftSelectUI[i]->Sprite::SetSize({ 128, 128 });
		m_leftSelectUI[i]->Sprite::SetTextureRect({ 0, 0 }, { 1024.0f, 1024.0f });

		m_rightSelectUI[i].reset(Sprite::Create(rightUITex_, { 420.0f, (i + 1) * 300.0f - 110.0f }));
		m_rightSelectUI[i]->Sprite::SetSize({ 128, 128 });
		m_rightSelectUI[i]->Sprite::SetTextureRect({ 0, 0 }, { 1024.0f, 1024.0f });

		m_volUI[i].reset(Sprite::Create(numberUITex_, { 260.0f, (i + 1) * 300.0f - 110.0f }));
		m_volUI[i]->Sprite::SetSize({ 128, 128 });
	}
	m_volUI[0]->Sprite::SetTextureRect(
		{ m_bgmVol * 10240.0f, 0.0f }, { 1024.0f, 1024.0f });
	m_volUI[1]->Sprite::SetTextureRect(
		{ m_seVol * 10240.0f, 0.0f }, { 1024.0f, 1024.0f });

	leftSelect = true;
	upSelect = true;
}

void Option::Update() {

	// ゲームパッドのインスタンスを取得
	Gamepad::Input();

	// カーソルの更新処理
	// CursorUpdate();

	if (isMenuOverlay_) {

		// 左上 ＜BGM Down＞
		if (leftSelect && upSelect) {

			// 上方向への入力
			if (input_->TriggerKey(DIK_W) || GetActionTrigger(U_SELECT)) {
			}

			// 下方向への入力
			if (input_->TriggerKey(DIK_S) || GetActionTrigger(D_SELECT)) {
				upSelect = false;
			}

			// 左方向への入力
			if (input_->TriggerKey(DIK_A) || GetActionTrigger(L_SELECT)) {
			}

			// 右方向への入力
			if (input_->TriggerKey(DIK_D) || GetActionTrigger(R_SELECT)) {
				leftSelect = false;
			}

			// 決定の入力
			if (input_->TriggerKey(DIK_RETURN) || GetActionTrigger(ACT)) {

				if (m_bgmVol > 0.0f) {
					m_bgmVol -= 0.1f;
					if (0.0f > m_bgmVol) {
						m_bgmVol = 0.0f;
					}
				}
			}
		}

		// 右上 ＜BGM Up＞
		if (!leftSelect && upSelect) {

			// 上方向への入力
			if (input_->TriggerKey(DIK_W) || GetActionTrigger(U_SELECT)) {
			}

			// 下方向への入力
			if (input_->TriggerKey(DIK_S) || GetActionTrigger(D_SELECT)) {
				upSelect = false;
			}

			// 左方向への入力
			if (input_->TriggerKey(DIK_A) || GetActionTrigger(L_SELECT)) {
				leftSelect = true;
			}

			// 右方向への入力
			if (input_->TriggerKey(DIK_D) || GetActionTrigger(R_SELECT)) {
			}

			// 決定の入力
			if (input_->TriggerKey(DIK_RETURN) || GetActionTrigger(ACT)) {

				if (m_bgmVol < 1.0f) {
					m_bgmVol += 0.1f;
				}
			}
		}

		// 左下 ＜SE Down＞
		if (leftSelect && !upSelect) {

			// 上方向への入力
			if (input_->TriggerKey(DIK_W) || GetActionTrigger(U_SELECT)) {
				upSelect = true;
			}

			// 下方向への入力
			if (input_->TriggerKey(DIK_S) || GetActionTrigger(D_SELECT)) {
			}

			// 左方向への入力
			if (input_->TriggerKey(DIK_A) || GetActionTrigger(L_SELECT)) {
			}

			// 右方向への入力
			if (input_->TriggerKey(DIK_D) || GetActionTrigger(R_SELECT)) {
				leftSelect = false;
			}

			// 決定の入力
			if (input_->TriggerKey(DIK_RETURN) || GetActionTrigger(ACT)) {

				if (m_seVol > 0.0f) {
					m_seVol -= 0.1f;
				}
			}
		}

		// 右下 ＜SE Up＞
		if (!leftSelect && !upSelect) {

			// 上方向への入力
			if (input_->TriggerKey(DIK_W) || GetActionTrigger(U_SELECT)) {
				upSelect = true;
			}

			// 下方向への入力
			if (input_->TriggerKey(DIK_S) || GetActionTrigger(D_SELECT)) {
			}

			// 左方向への入力
			if (input_->TriggerKey(DIK_A) || GetActionTrigger(L_SELECT)) {
				leftSelect = true;
			}

			// 右方向への入力
			if (input_->TriggerKey(DIK_D) || GetActionTrigger(R_SELECT)) {
			}

			// 決定の入力
			if (input_->TriggerKey(DIK_RETURN) || GetActionTrigger(ACT)) {

				if (m_seVol < 1.0f) {
					m_seVol += 0.1f;
				}
			}
		}

	}

	m_volUI[0]->Sprite::SetTextureRect(
	    {m_bgmVol * 10240.0f, 0.0f}, {1024.0f, 1024.0f});
	m_volUI[1]->Sprite::SetTextureRect(
	    {m_seVol * 10240.0f, 0.0f}, {1024.0f, 1024.0f});

	// ビューポート
	m_cursorSprite_->SetPosition({m_cursorPos.x, m_cursorPos.y});

	/*ImGui::Begin("Action");
	ImGui::Text("JUMP = %d", m_InputButton[ActCode::JUMP]);
	ImGui::Text("DASH = %d", m_InputButton[ActCode::DASH]);
	ImGui::Text("ACT = %d", m_InputButton[ActCode::ACT]);
	ImGui::Text("MENU = %d", m_InputButton[ActCode::MENU]);
	ImGui::Text("CANCEL = %d", m_InputButton[ActCode::CANCEL]);
	ImGui::End();

	ImGui::Begin("Option");
	ImGui::Text("Cursor_X = %d", m_cursorPos.x);
	ImGui::Text("Cursor_Y = %d", m_cursorPos.y);
	ImGui::Text("Frame rate = %3.0f fps", ImGui::GetIO().Framerate);
	ImGui::End();
	Gamepad::Draw();*/
}

void Option::Draw() {
	if (isMenuOverlay_) {
		m_menuSprite->Draw();

		// 左上 ＜BGM Down＞
		if (leftSelect && upSelect) {

			m_leftSelectUI[0]->SetColor({
			    1.0f,
			    1.0f,
			    1.0f,
			    1.0f,
			});
			m_leftSelectUI[1]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_rightSelectUI[0]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_rightSelectUI[1]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
		}

		// 右上 ＜BGM Up＞
		if (!leftSelect && upSelect) {
			m_leftSelectUI[0]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_leftSelectUI[1]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_rightSelectUI[0]->SetColor({
			    1.0f,
			    1.0f,
			    1.0f,
			    1.0f,
			});
			m_rightSelectUI[1]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
		}

		// 左下 ＜SE Down＞
		if (leftSelect && !upSelect) {
			m_leftSelectUI[0]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_leftSelectUI[1]->SetColor({
			    1.0f,
			    1.0f,
			    1.0f,
			    1.0f,
			});
			m_rightSelectUI[0]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_rightSelectUI[1]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
		}

		// 右下 ＜SE Up＞
		if (!leftSelect && !upSelect) {
			m_leftSelectUI[0]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_leftSelectUI[1]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_rightSelectUI[0]->SetColor({
			    0.5f,
			    0.5f,
			    0.5f,
			    0.5f,
			});
			m_rightSelectUI[1]->SetColor({
			    1.0f,
			    1.0f,
			    1.0f,
			    1.0f,
			});
		}

		for (int i = 0; i < 2; i++) {

			m_leftSelectUI[i]->Draw();
			m_rightSelectUI[i]->Draw();
			m_volUI[i]->Draw();
		}
	}

	// m_cursorSprite_->Draw();
}

bool Option::GetActionTrigger(ActCode act) {

	switch (act) {

	case SPECIAL:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::SPECIAL] == i) {
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
	case L_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::L_SELECT] == i) {
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
	case R_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::R_SELECT] == i) {
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
	case U_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::U_SELECT] == i) {
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
	case D_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::D_SELECT] == i) {
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
	case UI_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::UI_SELECT] == i) {
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

bool Option::GetActionLongPush(ActCode act) {

	switch (act) {

	case SPECIAL:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::SPECIAL] == i) {
				switch (i) {
				case 0:
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
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
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
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
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
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
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
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
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case L_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::L_SELECT] == i) {
				switch (i) {
				case 0:
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case R_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::R_SELECT] == i) {
				switch (i) {
				case 0:
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case U_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::U_SELECT] == i) {
				switch (i) {
				case 0:
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case D_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::D_SELECT] == i) {
				switch (i) {
				case 0:
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
						return true;
					}
					break;
				}
			}
		}

		break;
	case UI_SELECT:

		for (int i = 0; i < 16; i++) {

			if (m_InputButton[ActCode::UI_SELECT] == i) {
				switch (i) {
				case 0:
					if (Gamepad::LongPush(Gamepad::Button::UP)) {
						return true;
					}
					break;
				case 1:
					if (Gamepad::LongPush(Gamepad::Button::DOWN)) {
						return true;
					}
					break;
				case 2:
					if (Gamepad::LongPush(Gamepad::Button::LEFT)) {
						return true;
					}
					break;
				case 3:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT)) {
						return true;
					}
					break;
				case 4:
					if (Gamepad::LongPush(Gamepad::Button::START)) {
						return true;
					}
					break;
				case 5:
					if (Gamepad::LongPush(Gamepad::Button::BACK)) {
						return true;
					}
					break;
				case 6:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_THUMB)) {
						return true;
					}
					break;
				case 7:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_THUMB)) {
						return true;
					}
					break;
				case 8:
					if (Gamepad::LongPush(Gamepad::Button::LEFT_SHOULDER)) {
						return true;
					}
					break;
				case 9:
					if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
						return true;
					}
					break;
				case 12:
					if (Gamepad::LongPush(Gamepad::Button::A)) {
						return true;
					}
					break;
				case 13:
					if (Gamepad::LongPush(Gamepad::Button::B)) {
						return true;
					}
					break;
				case 14:
					if (Gamepad::LongPush(Gamepad::Button::X)) {
						return true;
					}
					break;
				case 15:
					if (Gamepad::LongPush(Gamepad::Button::Y)) {
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
		m_cursorPos.y -= m_cursorVel.y;
	}
	if (Gamepad::getStick(Gamepad::Stick::LEFT_Y) <= -3000) {
		m_cursorPos.y += m_cursorVel.y;
	}

	if (m_cursorPos.x < 10) {
		m_cursorPos.x = 10;
	}
	if (m_cursorPos.x > 1270) {
		m_cursorPos.x = 1270;
	}
	if (m_cursorPos.y < 10) {
		m_cursorPos.y = 10;
	}
	if (m_cursorPos.y > 710) {
		m_cursorPos.y = 710;
	}

	m_cursorSprite_->SetPosition({m_cursorPos.x, m_cursorPos.y});
}

Vector2 Option::GetCursorPos() { return Vector2(m_cursorPos.x, m_cursorPos.y); }

Vector2 Option::GetCursorRad() { return Vector2{16, 16}; }

void Option::SetControllType(TYPE type) {

	switch (type) {
	case Option::A:

		m_InputButton[ActCode::ACT] = (int)Gamepad::Button::A;
		m_InputButton[ActCode::CANCEL] = (int)Gamepad::Button::B;

		break;
	case Option::B:

		m_InputButton[ActCode::ACT] = (int)Gamepad::Button::B;
		m_InputButton[ActCode::CANCEL] = (int)Gamepad::Button::A;

		break;
	default:

		break;
	}
}