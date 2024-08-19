#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <fstream>
#include <cassert>
#include "imgui.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete debugCamera_;
	delete skydome_;
	delete modelSkydome_;
	delete railCamera_;
	delete modelParticle_;
	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	for (Particle* particle : particles_) {
		delete particle;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	// 3Dモデルデータの生成
	model_ = Model::Create();
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 650;
	viewProjection_.Initialize();
	// 自キャラの生成
	player_ = new Player();
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	// 天球の生成
	skydome_ = new Skydome();
	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_);
	// レールカメラの生成
	railCamera_ = new RailCamera();
	railCamera_->Initialize();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());
	// 自キャラの初期化
	Vector3 playerPosition(0, 0, 50);
	player_->Initialize(model_, textureHandle_, playerPosition);

	std::string enemyDataFilePath[3]{
		"Resources/enemyPop1.csv",
		"Resources/enemyPop2.csv",
		"Resources/enemyPop3.csv"
	};
	for (int i = 0; i < 3; i++) {
		LoadEnemyPopData(enemyDataFilePath[i], enemyPopCommands_[i]);
	}
	//レティクルのテクスチャ
	TextureManager::Load("2dReticle.png");
	modelParticle_ = Model::CreateFromOBJ("enemyExplosion", true);
	praticleTexture_ = TextureManager::Load("white1x1.png");
}

void GameScene::AddEnemyBullet(Enemy* enemy) {
	// リストに登録する
	enemys_.push_back(enemy);
}

void GameScene::LoadEnemyPopData(std::string& filePath, std::stringstream& enemyPopCommands) {
	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());
	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();
	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands(std::stringstream& enemyPopCommands) { 
	if (waitEnemy_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			// 待機完了
			waitEnemy_ = false;
		}
		return;
	}
	//1行文の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (std::getline(enemyPopCommands, line)) {
		//1行文の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);
		std::string word;
		//,区切りで行の先頭文字列を取得
		std::getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}
		if (word.find("POP") == 0) {
			//x
			std::getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			//y
			std::getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			//z
			std::getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());
			//ここに敵の座標を初期化する処理
			// 敵の生成
			enemy_ = new Enemy();
			enemy_->Initialize(model_, textureHandle_);
			enemy_->SetGameScene(this);
			enemy_->SetPlayer(player_);
			enemy_->SetEnemyPosition(Vector3(x, y, z) + railCamera_->GetWorldTransform().translation_);
			enemys_.push_back(enemy_);

		} else if (word.find("WAIT") == 0) {
			std::getline(line_stream, word, ',');
			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			waitEnemy_ = true;
			waitTime_ = waitTime;

			//コマンドループを抜ける
			break;
		}
	}
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	//リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}
#endif
	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.UpdateMatrix();
	}
	CheckAllCollisions();
	//レールカメラ
	railCamera_->Update();
	viewProjection_.matView = railCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	// 天球の更新
	skydome_->Update();
	// 自キャラの更新
	player_->Update(railCamera_->GetViewProjection());

	// 敵のcsvファイル読み込みとシーン切り替え
	switch (enemyScene) {
	case EnemyScene::Stage1:
		UpdateEnemyPopCommands(enemyPopCommands_[0]);
		break;
	case EnemyScene::Stage2:
		UpdateEnemyPopCommands(enemyPopCommands_[1]);
		break;
	case EnemyScene::Stage3:
		UpdateEnemyPopCommands(enemyPopCommands_[2]);
		break;
	default:
		break;
	}

	// 敵の更新とパーティクルの初期化
	for (Enemy* enemy : enemys_) {
		enemy->Update(railCamera_->GetViewProjection());
		if (enemy->IsDead()) {
			Particle* newParticle = new Particle();
			newParticle->Initialize(modelParticle_, praticleTexture_, enemy->GetWorldPosition(), 10, 600.0f);
			particles_.push_back(newParticle);
		}
	}
	// 敵弾の更新とパーティクルの初期化
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
		if (bullet->IsDead()) {
			Particle* newParticle = new Particle();
			newParticle->Initialize(modelParticle_, praticleTexture_, bullet->GetWorldPosition(), 5, 300.0f);
			particles_.push_back(newParticle);
		}
	}

	// パーティクルの更新
	for (Particle* particle : particles_) {
		particle->Update();
	}
	// 条件が揃ったら次のシーンに移動
	UpdateAndCheckScene(EnemyScene::Stage1, EnemyScene::Stage2);
	UpdateAndCheckScene(EnemyScene::Stage2, EnemyScene::Stage3);

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	particles_.remove_if([](Particle* particle) {
		if (!particle->IsActive()) {
			delete particle;
			return true;
		}
		return false;
	});
	Timer_--;
	if (Timer_ < 0) {
		Fire();
		Timer_ = kFireInterval;
	}
	player_->SetEnemy(enemys_);
	player_->SetRailCamera(railCamera_);
}

void GameScene::UpdateAndCheckScene(EnemyScene currentScene, EnemyScene nextScene) {
	bool sceneChanged = false;

	if (enemyScene == currentScene && !sceneChanged) {
		for (Enemy* enemy : enemys_) {
			if (railCamera_->GetWorldTransform().translation_.z > enemy->GetWorldPosition().z) {
				enemyScene = nextScene;
				sceneChanged = true; // フラグを立てる
			}
		}
		if (sceneChanged) {
			for (Enemy* enemy : enemys_) {
				enemy->onCollision();
			}
		}
		if (!enemys_.empty()) {
			bool allEnemiesDead = true; // 全ての敵が死んでいるかを確認するフラグ

			for (Enemy* enemy : enemys_) {
				if (!enemy->IsDead()) {     // もし1体でも敵が生きていたら
					allEnemiesDead = false; // フラグをfalseにする
					break;                  // ループを抜ける
				}
			}
			if (allEnemiesDead) { 
				for (Enemy* enemy : enemys_) {
					delete enemy;
				}                       // 全ての敵が死んでいた場合
				enemys_.clear();                 // リストをクリア
				enemyScene = nextScene; // 次のステージに切り替える
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullet();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemyBullets_;
#pragma region // 自キャラと敵弾の当たり判定
	for (EnemyBullet* enemyBullet : enemyBullets) {
		CheckCollisionPair(player_, enemyBullet);
	}
#pragma endregion
#pragma region // 自弾と敵キャラの当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (Enemy* enemy : enemys_) {
			CheckCollisionPair(enemy, playerBullet);
		}
	}
#pragma endregion
#pragma region // 自弾と敵弾の当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets) {
			CheckCollisionPair(playerBullet, enemyBullet);
		}
	}
#pragma endregion
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	Vector3 posA = colliderA->GetWorldPosition();
	Vector3 posB = colliderB->GetWorldPosition();
	float radiusA = colliderA->GetRadius();
	float radiusB = colliderB->GetRadius();
	if (circleCollision(posA, posB, radiusA, radiusB)) {
		colliderA->onCollision();
		colliderB->onCollision();
	}
}

void GameScene::Fire() {
	// 弾の速度
	for (Enemy* enemy : enemys_) {
		const float kBulletSpeed = 0.5f;

		Vector3 worldPlayerPos = player_->GetWorldPosition();
		Vector3 worldEnemyPos = enemy->GetWorldPosition();
		Vector3 Vector = Subtract(worldPlayerPos, worldEnemyPos);
		Vector3 normaLizeVector = Normalize(Vector);
		Vector3 velocity = Multiply(kBulletSpeed, normaLizeVector);

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->SetPlayer(player_);
		newBullet->Initialize(model_, enemy->GetWorldPosition(), velocity);

		enemyBullets_.push_back(newBullet);
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	skydome_->Draw(viewProjection_);
	// 自キャラの描画
	player_->Draw(viewProjection_);
	//敵の描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}
	for (Particle* particle : particles_) {
		particle->Draw(viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	player_->DrawUI();
	for (Enemy* enemy : enemys_) {
		enemy->DrawUI();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}