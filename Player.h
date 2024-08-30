#pragma once
#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "WorldTransform.h"
#include "MT3.h"
#include <list>
#include <Sprite.h>
#include <WinApp.h>
#include <Audio.h>
#include "Collider.h"

class Enemy;

class RailCamera;

/// <summary>
/// 自キャラ
/// </summary>
class Player : public Collider {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Vector3 playerPos, Audio* audio);
	void BulletInitialize(float bulletSpeed, Vector3 worldPosition, Vector3 velocity);
	// ワールド座標を取得
	Vector3 GetWorldPosition() override;
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetWorldReticlePosition();
	float GetBulletAttackNum() { return bulletAttackNum_; }
	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	void SetParent(const WorldTransform* parent);
	void SetEnemy(std::list<Enemy*> enemy) { enemys_ = enemy; }
	void SetRailCamera(RailCamera* railCamera) { railCamera_ = railCamera; }
	void SetBulletModel(Model* model) { bulletModel_ = model; }
	void onCollisionBullet(float i) { targetReticleTima_ += i; }

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection& viewProjection);
	void ReticleUpdate(const ViewProjection& viewProjection);
	void ReticleCollision(const ViewProjection& viewProjection);
	// 攻撃
	void Attack(const ViewProjection& viewProjection);
	void TargetAttack();
	// 回避
	void Avoidance();
	//衝突を検出したら呼び出しされるコールバック関数
	void onCollision() override;
	bool IsDead() const { return isDead_; }
	float GetRadius() override;
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullet() const { return bullets_; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション(参照渡し)</param>
	void Draw(ViewProjection& viewProjection);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション(参照渡し)</param>
	void DrawUI();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	bool isDead_ = false;
	Sprite* spriteHPGauge_ = nullptr;
	Sprite* spriteHPGaugeFrame_ = nullptr;
	float playerHP_;
	bool playerLeftRotate_;
	float rotateLeftTime_;
	bool playerRightRotate_;
	float rotateRightTime_;
	// モデル
	Model* playerModel_ = nullptr;
	Model* bulletModel_ = nullptr;
	// キーボード入力
	Input* input_ = nullptr;
	// サウンドデータ
	Audio* audio_ = nullptr;
	uint32_t bulletSound_ = 0;
	uint32_t reticleSound_ = 0;
	uint32_t playerDamage_ = 0;
	uint32_t avoidanceSound_ = 0;
	//複数弾
	std::list<PlayerBullet*> bullets_;
	float bulletAttackNum_ = 0;
	//3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;
	// ロックオンゲージの画像
	Sprite* spriteGauge_ = nullptr;
	Sprite* spriteGaugeFrame_ = nullptr;
	Sprite* spriteHitu_ = nullptr;
	// ターゲットレティクルの待ち時間
	float targetReticleTima_ = 0;
	//敵をターゲットしているかどうか
	bool isTargetingEnemy_ = false;
	//2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;
	Vector2 spritePosition_ = {};
	// 敵のリスト管理
	std::list<Enemy*> enemys_;
	RailCamera* railCamera_;
};