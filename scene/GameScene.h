#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "Enemy.h"
#include "Particle.h"
#include "Skydome.h"
#include "Sprite.h"
#include "RailCamera.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 敵を追加する
	/// </summary>
	void AddEnemyBullet(Enemy* enemy);
	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData(std::string& filePath, std::stringstream& enemyPopCommands);
	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands(std::stringstream& enemyPopCommands);

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	static const int kFireInterval = 60;

	private:
	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータ
	Model* model_ = nullptr;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 自キャラ
	Player* player_ = nullptr;
	//敵
	Enemy* enemy_ = nullptr;
	int32_t waitTime_;
	bool waitEnemy_;
	std::list<Enemy*> enemys_;
	//敵発生コマンド
	std::stringstream enemyPopCommands_[3];
	enum EnemyScene {
		Stage1,
		Stage2,
		Stage3,
	};
	EnemyScene enemyScene = EnemyScene::Stage1;
	void UpdateAndCheckScene(EnemyScene currentScene, EnemyScene nextScene);
	//  複数弾
	std::list<EnemyBullet*> enemyBullets_;
	// 発射タイマー
	int32_t Timer_ = 0;
	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
	//レールカメラ
	RailCamera* railCamera_ = nullptr;
	// パーティクル
	std::list<Particle*> particles_; 
	Model* modelParticle_ = nullptr;
	uint32_t praticleTexture_ = 0;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};