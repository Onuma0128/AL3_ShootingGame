#include "RailCamera.h"
#include "MT3.h"
#include "ImGuiManager.h"

void RailCamera::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.translation_ = {0, 0, 0};
	worldTransform_.rotation_ = {0, 0, 0};
	//worldTransform_.matWorld_ = Inverse(view.matView);
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 650;
	viewProjection_.Initialize();

}

void RailCamera::Update() {
	Vector3 move{0.0f, 0.0f, 0.01f};
	Vector3 rad{0.0f, 0.0001f, 0.0f};
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	worldTransform_.rotation_ = Add(worldTransform_.rotation_, rad);
	worldTransform_.matWorld_ = MakeAfineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
}
