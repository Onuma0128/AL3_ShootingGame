#pragma once
#include "Model.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cassert"
#include "Collider.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider {
public:
	/// 初期化
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	void TargetInitialize(Model* model, const Vector3& position, const Vector3& targetPosition);
	// ワールド座標を取得
	Vector3 GetWorldPosition() override;
	void SetParent(const WorldTransform* parent);
	void SetRotate(const Vector3 rotate);
	bool GetIsTarget() { return isTargeting_; }
	void SetTargetPosition(const Vector3& position);

	/// 更新
	void Update();
	// 衝突を検出したら呼び出しされるコールバック関数
	void onCollision() override;
	float GetRadius() override;
	bool IsDead() const { return isDead_; }

	/// 描画
	void Draw(const ViewProjection& viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデルのポインター
	Model* model_ = nullptr;
	//弾の速度
	Vector3 velocity_;
	// ターゲット座標
	Vector3 playerPosition_{};
	Vector3 targetPosition_{};
	float lerpTimer_;
	float lerpDuration_;
	// ターゲットしているかフラグ
	bool isTargeting_ = false;
	//弾の寿命
	static const int32_t kLifeTime = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = kLifeTime;
	//デスフラグ
	bool isDead_ = false;
};