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
#include "MiniMap.h"
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
	bool IsFinished() const { return finished_; }
	uint32_t GetGameSceneBGM() const { return voiceHandle_; }
	float GetGameScore() const { return gameScore_; }

	void EnemySceneInitialize();
	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData(std::string& filePath, std::stringstream& enemyPopCommands);
	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands(std::stringstream& enemyPopCommands);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void RandomEnemyScene(int i);
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

private:
	// 終了フラグ
	float gameScore_ = 0;
	bool finished_ = false;
	// メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	// ゲームシーンのBGM
	uint32_t gameSceneBGM_ = 0;
	uint32_t voiceHandle_ = 0;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータ
	Model* model_ = nullptr;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 自キャラ
	Player* player_ = nullptr;
	Model* playerModel_ = nullptr;
	Model* playerBulletModel_ = nullptr;
	//敵
	Enemy* enemy_ = nullptr;
	uint32_t enemyExplosionSound_ = 0;
	int32_t waitTime_;
	bool waitEnemy_;
	std::list<Enemy*> enemys_;
	Model* enemyModel_ = nullptr;
	//敵発生コマンド
	std::stringstream enemyPopCommands_[9];
	enum EnemyScene {
		Stage1,
		Stage2,
		Stage3,
		Stage4,
		Stage5,
		Stage6,
		Stage7,
		Stage8,
		Stage9,
	};
	EnemyScene enemyScene = EnemyScene::Stage1;
	std::string enemyDataFilePath_[9] = {};
	void UpdateAndCheckScene(EnemyScene currentScene);
	//  複数弾
	std::list<EnemyBullet*> enemyBullets_;
	Model* enemyBulletModel_ = nullptr;
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
	// ミニマップ
	MiniMap* miniMap_;
	std::list<MiniMap*> miniMaps_;
	uint32_t texturePlayerSprite_ = 0;
	uint32_t textureEnemySprite_ = 0;
	uint32_t textureMiniMapBaseSprite_ = 0;
	Sprite* miniMapBaseSprite_{};

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};