#include "TitleScene.h"

void TitleScene::Initialize() {}

void TitleScene::Update() {
	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		finished_ = true;
	}
}

void TitleScene::Draw() {}
