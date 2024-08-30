#include "RailCamera.h"
#include "MT3.h"
#include <algorithm>

void RailCamera::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.translation_ = {0, 0, 0};
	worldTransform_.rotation_ = {0, 0, 0};
	rotate_ = 0;
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 1000;
	viewProjection_.Initialize();
}

void RailCamera::PlayerDamageInitialze() {
	cameraDefaultPos_ = worldTransform_.translation_;
	isPlayerDamage_ = true;
	randTime_ = 0.5f;
}

void RailCamera::SetTarget(const Vector3 target) { target_ = target; }

void RailCamera::SetRotate(const float rotate) { rotate_ = rotate; }

void RailCamera::Update() {
	PlayerDamageUpdate();
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
}

void RailCamera::PlayerDamageUpdate() {
	float shakeIntensity = 0.1f;
	if (isPlayerDamage_ && randTime_ > 0) {
		randTime_ -= 1.0f / 60.0f;
		float scale = randTime_ / (30.0f / 60.0f);
		Vector3 playerRand { 
			.x = (rand() % 11 - 5) * shakeIntensity * scale,
			.y = (rand() % 11 - 5) * shakeIntensity * scale,
			.z = 0
		};
		worldTransform_.translation_ = Add(worldTransform_.translation_, playerRand);
	}
	if (randTime_ <= 0) {
		isPlayerDamage_ = false;
		worldTransform_.translation_ = cameraDefaultPos_;
	}
}
