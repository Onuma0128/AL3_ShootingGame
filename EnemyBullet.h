#pragma once
#include "Model.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collider.h"

class Player;

class EnemyBullet : public Collider{
public:
	/// 初期化
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	void SetPlayer(Player* player) { player_ = player; }
	// ワールド座標を取得
	Vector3 GetWorldPosition() override;

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
	// 弾の速度
	Vector3 velocity_;
	// 弾の寿命
	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	Player* player_ = nullptr;
	float t = 0;
};
