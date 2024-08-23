#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include <list>
#include <Sprite.h>
#include "EnemyBullet.h"
#include <WinApp.h>
#include "BaseEnemyState.h"
#include "Collider.h"

//GameSceneの前方宣言
class GameScene;

// 自機クラスの前方宣言
class Player;

/// <summary>
/// 敵
/// </summary>

class Enemy : public Collider{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);
	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemyPosition(const Vector3& pos);
	void SetEnemySpeed(const Vector3& move);
	// ワールド座標を取得
	Vector3 GetWorldPosition() override;
	Vector3 GetEnemySpeed() { return move_; }
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection& viewProjection);
	// 状態変更
	void ChangeState(std::unique_ptr<BaseEnemyState> state);

	void EnemyMove(const Vector3& move);
	// 衝突を検出したら呼び出しされるコールバック関数
	void onCollision() override;
	float GetRadius() override;
	// 弾リストを取得
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void SetIsTargetingEnemy(bool isTargetingEnemy) { isTargetingEnemy_ = isTargetingEnemy; }
	bool GetIsTargetingEnemy();

	/// <summary>
	/// 弾発射
	/// </summary>
	//void Fire();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション(参照渡し)</param>
	void Draw(ViewProjection& viewProjection);

	void DrawUI();

public:
	//static const int kFireInterval = 60;
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	Vector3 move_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 敵の状態
	std::unique_ptr<BaseEnemyState> state_;
	//自キャラ
	Player* player_ = nullptr;
	//ゲームシーン
	GameScene* gameScene_ = nullptr;
	// デスフラグ
	bool isDead_ = false;
	//2Dレティクル
	Sprite* spriteTargetReticle_ = nullptr;
	bool isTargetingEnemy_;

};