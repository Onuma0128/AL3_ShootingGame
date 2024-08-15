#include "PlayerBullet.h"

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

Vector3 PlayerBullet::GetWorldPosition() {

	return Transform(Vector3{0, 0, 0}, worldTransform_.matWorld_);
}

void PlayerBullet::SetParent(const WorldTransform* parent) { 
	worldTransform_.parent_ = parent; 
}

void PlayerBullet::Update() {
	worldTransform_.translation_ = Add(velocity_, worldTransform_.translation_);
	worldTransform_.UpdateMatrix();
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::onCollision() { isDead_ = true; }

float PlayerBullet::GetRadius() { return 1.0f; }

void PlayerBullet::Draw(const ViewProjection& viewProjection) { model_->Draw(worldTransform_, viewProjection, textureHandle_); }