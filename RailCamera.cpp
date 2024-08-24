#include "RailCamera.h"
#include "MT3.h"
#include <algorithm>
#include "ImGuiManager.h"

void RailCamera::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.translation_ = {0, 0, 0};
	worldTransform_.rotation_ = {0, 0, 0};
	rotate_ = 0;
	//worldTransform_.matWorld_ = Inverse(view.matView);
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 1000;
	viewProjection_.Initialize();
}

void RailCamera::SetTarget(const Vector3 target) { target_ = target; }

void RailCamera::SetRotate(const float rotate) { rotate_ = rotate; }

void RailCamera::Update() {
	Vector3 move{0.0f, 0.0f, 0.0f};
	Vector3 rad{0.0f, 0.0f, 0.0f};
	// カメラの回転の速さ
	const float kRotateSpeed = 0.01f;
	// ジョイスティックの入力処理
	XINPUT_STATE joystate;
	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		rad.x -= (float)joystate.Gamepad.sThumbLY / SHRT_MAX * kRotateSpeed;
		rad.y += (float)joystate.Gamepad.sThumbLX / SHRT_MAX * kRotateSpeed;
	}
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	worldTransform_.rotation_.y = worldTransform_.rotation_.y + rotate_;
	worldTransform_.rotation_ = Add(worldTransform_.rotation_, rad);
	worldTransform_.rotation_.x = std::clamp(worldTransform_.rotation_.x, -0.5f, 0.5f);
	worldTransform_.matWorld_ = MakeAfineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = LookAt(worldTransform_.translation_, target_, {0.0f, 1.0f, 0.0f});

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
}
