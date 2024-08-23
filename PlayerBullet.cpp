#include "PlayerBullet.h"
#include "imgui.h"

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("sample.png");
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void PlayerBullet::TargetInitialize(Model* model, const Vector3& position, const Vector3& targetPosition) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("sample.png");
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	playerPosition_ = position;
	// 引数で受け取った速度をメンバ変数に代入
	targetPosition_ = targetPosition;
	//targetPosition_ = targetPosition;
	isTargeting_ = true;

	lerpTimer_ = 0.0f;
	//lerpDuration_ = 1.0f;
}

Vector3 PlayerBullet::GetWorldPosition() {

	return Transform(Vector3{0, 0, 0}, worldTransform_.matWorld_);
}

void PlayerBullet::SetParent(const WorldTransform* parent) { 
	worldTransform_.parent_ = parent; 
}

void PlayerBullet::SetTargetPosition(const Vector3& position) {
	targetPosition_ = position;
}

void PlayerBullet::Update() {
	if (!isTargeting_) {
		worldTransform_.translation_ = Add(velocity_, worldTransform_.translation_);
		worldTransform_.UpdateMatrix();
		// 時間経過でデス
		if (--deathTimer_ <= 0) {
			isDead_ = true;
		}
	} else {
		// Lerpを使った補間でターゲットに向かう
		lerpTimer_ += 1.0f / 30.0f;

		float t = lerpTimer_ / 1.0f;

		float targetLength = VectorDistance(playerPosition_, targetPosition_) / 3.0f;

		// カーブを描くための中間点を設定（コントロールポイント）
		Vector3 controlPoint{};
		if (playerPosition_.x < targetPosition_.x) {
			controlPoint.x = playerPosition_.x + targetLength;
		} else {
			controlPoint.x = playerPosition_.x - targetLength;
		}
		if (playerPosition_.y < targetPosition_.y) {
			controlPoint.y = playerPosition_.y + targetLength;
		} else {
			controlPoint.y = playerPosition_.y - targetLength;
		}
		controlPoint.z = (playerPosition_.z + targetPosition_.z) / 2.0f;

		// 二次ベジェ曲線の計算
		Vector3 pointA = Lerp(worldTransform_.translation_, controlPoint, t);
		Vector3 pointB = Lerp(controlPoint, targetPosition_, t);
		Vector3 newPosition = Lerp(pointA, pointB, t);

		worldTransform_.translation_ = newPosition;

		worldTransform_.UpdateMatrix();

		// 時間経過でデス
		if (--deathTimer_ <= 0) {
			isDead_ = true;
		}

		ImGui::Text("%f", targetLength);
	}
}

void PlayerBullet::onCollision() { 
	isDead_ = true;
	isTargeting_ = false;
}

float PlayerBullet::GetRadius() { return 1.0f; }

void PlayerBullet::Draw(const ViewProjection& viewProjection) { model_->Draw(worldTransform_, viewProjection, textureHandle_); }