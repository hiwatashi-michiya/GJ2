#include "Gamepad.h"
//#include "Novice.h"
#ifdef _DEBUG
#include"ImGuiManager.h"
#endif // _DEBUG
#include <limits.h>

XINPUT_STATE Gamepad::state = { 0 };
XINPUT_VIBRATION Gamepad::vibration = { 0 };
unsigned short Gamepad::preButton = 0;

void Gamepad::Input() {
	preButton = state.Gamepad.wButtons;
    XInputGetState(0, &state);
}

bool Gamepad::getButton(const Button& type) {
    return (state.Gamepad.wButtons >> static_cast<unsigned short>(type)) % 2 == 1;
}

bool Gamepad::getPreButton(const Button& type) {
	return (preButton >> static_cast<unsigned short>(type)) % 2 == 1;
}

bool Gamepad::Pushed(const Button& type) {
	return getButton(type) && !getPreButton(type);
}

bool Gamepad::LongPush(const Button& type) {
	return getButton(type) && getPreButton(type);
}

bool Gamepad::Released(const Button& type) {
	return !getButton(type) && getPreButton(type);
}

unsigned char Gamepad::getTriger(const Triger& type) {
	switch (type)
	{
	case Gamepad::Triger::LEFT:
		return state.Gamepad.bLeftTrigger;
		break;

	case Gamepad::Triger::RIGHT:
		return state.Gamepad.bRightTrigger;
		break;

	default:
		return 0;
		break;
	}
}

short Gamepad::getStick(const Stick& type) {
	switch (type)
	{
	case Gamepad::Stick::LEFT_X:
		return state.Gamepad.sThumbLX;
		break;

	case Gamepad::Stick::LEFT_Y:
		return state.Gamepad.sThumbLY;
		break;

	case Gamepad::Stick::RIGHT_X:
		return state.Gamepad.sThumbRX;
		break;

	case Gamepad::Stick::RIGHT_Y:
		return state.Gamepad.sThumbRY;
		break;

	default:
		return 0;
		break;
	}
}

void Gamepad::isVibration(const int& flag) {
	if(flag >= 1){
		vibration.wLeftMotorSpeed = USHRT_MAX;
		vibration.wRightMotorSpeed = USHRT_MAX;
		XInputSetState(0, &vibration);
	}
	else {
		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;
		XInputSetState(0, &vibration);
	}
}

void Gamepad::Draw() {
#ifdef _DEBUG
	ImGui::Begin("GamePad");
	ImGui::Text("LeftX = %d", getStick(Stick::LEFT_X));
	ImGui::Text("LeftY = %d", getStick(Stick::LEFT_Y));
	ImGui::Text("RightX = %d", getStick(Stick::RIGHT_X));
	ImGui::Text("RightY = %d", getStick(Stick::RIGHT_Y));
	ImGui::Text("UP = %d", getButton(Button::UP));
	ImGui::Text("DOWN = %d", getButton(Button::DOWN));
	ImGui::Text("LEFT = %d", getButton(Button::LEFT));
	ImGui::Text("RIGHT = %d", getButton(Button::RIGHT));
	ImGui::Text("START = %d", getButton(Button::START));
	ImGui::Text("BACK = %d", getButton(Button::BACK));
	ImGui::Text("LEFT_THUMB = %d", getButton(Button::LEFT_THUMB));
	ImGui::Text("RIGHT_THUMB = %d", getButton(Button::RIGHT_THUMB));
	ImGui::Text("LEFT_SHOULDER = %d", getButton(Button::LEFT_SHOULDER));
	ImGui::Text("RIGHT_SHOULDER = %d", getButton(Button::RIGHT_SHOULDER));
	ImGui::Text("A = %d", getButton(Button::A));
	ImGui::Text("B = %d", getButton(Button::B));
	ImGui::Text("X = %d", getButton(Button::X));
	ImGui::Text("Y = %d", getButton(Button::Y));
	ImGui::Text("LEFT_TRIGER = %d", getTriger(Triger::LEFT));
	ImGui::Text("RIGHT_TRIGER = %d", getTriger(Triger::RIGHT));
	ImGui::End();
#endif // _DEBUG
}