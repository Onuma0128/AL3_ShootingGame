#include "Enemy.h"
#include "Player.h"
#include "MT3.h"
#include <cassert>
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"

Enemy::~Enemy() { 
	delete spriteTargetReticle_;
}

void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_.x = 10.0f;
	worldTransform_.translation_.y = 2.0f;
	worldTransform_.translation_.z = 50.0f;
	ChangeState(std::make_unique<EnemyStateApproach>(this));
	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("2dReticle.png");
	// スプライト生成
	spriteTargetReticle_ = Sprite::Create(textureReticle, Vector2{0.0f, 0.0f}, Vector4{1.0f, 0.0f, 0.0f, 1.0f}, Vector2{0.5f, 0.5f});
	isTargetingEnemy_ = false;
}

void Enemy::SetEnemyPosition(const Vector3& pos) { worldTransform_.translation_ = pos; }

Vector3 Enemy::GetWorldPosition() { 
	return Transform(Vector3{0, 0, 0}, worldTransform_.matWorld_); 
}

void Enemy::Update(const ViewProjection& viewProjection) {
	if (worldTransform_.translation_.z <= 0) {
		ChangeState(std::make_unique<EnemyStateLeave>(this));
	}

	state_->Update();
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
	// 敵をスクリーン座標に変換
	Vector3 positionEnemy = Transform(worldTransform_.translation_, matViewProjectionViewport);
	spriteTargetReticle_->SetPosition(Vector2(positionEnemy.x, positionEnemy.y));
	spriteTargetReticle_->SetColor(Vector4{1.0f, 0.0f, 0.0f, 1.0f});
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) { state_ = std::move(state); }

void Enemy::EnemyMove(const Vector3& move) { worldTransform_.translation_ = Add(worldTransform_.translation_, move); }

void Enemy::onCollision() { 
	isDead_ = true; 
	isTargetingEnemy_ = false;
}

float Enemy::GetRadius() { return 1.0f; }

bool Enemy::GetIsTargetingEnemy() { return isTargetingEnemy_; }

void Enemy::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Enemy::DrawUI() {
	if (isTargetingEnemy_) {
		spriteTargetReticle_->Draw();
	}
}
