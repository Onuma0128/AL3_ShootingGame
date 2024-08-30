#include "Player.h"
#include <cassert>
#include "Enemy.h"
#include "RailCamera.h"
#include "math.h"

Player::~Player() { 
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
	delete spriteGauge_;
	delete spriteGaugeFrame_;
	delete spriteHitu_;
	delete spriteHPGauge_;
	delete spriteHPGaugeFrame_;
}

void Player::Initialize(Model* model, Vector3 playerPos, Audio* audio) {
	// NULLポインタチェック
	assert(model);
	playerModel_ = model;
	worldTransform_.translation_ = playerPos;
	worldTransform_.Initialize();
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();
	playerHP_ = 10;
	playerLeftRotate_ = false;
	rotateLeftTime_ = 0.0f;
	playerRightRotate_ = false;
	rotateRightTime_ = 0.0f;
	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("2dReticle.png");
	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, Vector2{640.0f, 250.0f}, Vector4{0.0f, 0.0f, 0.0f, 1.0f}, Vector2{0.5f, 0.5f});
	targetReticleTima_ = 1.0f;
	isTargetingEnemy_ = false;
	// ゲージ画像の初期化
	uint32_t textureGauge[4] = {
		TextureManager::Load("lockONGauge.png"),
		TextureManager::Load("lockONGaugeFrame.png"),
		TextureManager::Load("hitu.png"),
		TextureManager::Load("playerHPGauge.png")
	};
	spriteHPGauge_ = Sprite::Create(textureGauge[3], Vector2{48.0f, 652.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	spriteHPGaugeFrame_ = Sprite::Create(textureGauge[1], Vector2{208.0f, 652.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	spriteHPGaugeFrame_->SetSize(Vector2{352, 72.0f});

	spriteGauge_ = Sprite::Create(textureGauge[0], Vector2{48.0f, 68.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	spriteGaugeFrame_ = Sprite::Create(textureGauge[1], Vector2{208.0f, 68.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	spriteHitu_ = Sprite::Create(textureGauge[2], Vector2{48.0f, 56.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	spriteGaugeFrame_->SetSize(Vector2{352, 72.0f});
	spriteHitu_->SetSize(Vector2{64.0f, 64.0f});

	audio_ = audio;
	bulletSound_ = audio_->LoadWave("sound/bullet.wav");
	reticleSound_ = audio_->LoadWave("sound/reticle.wav");
	playerDamage_ = audio_->LoadWave("sound/playerDamage.mp3");
	avoidanceSound_ = audio_->LoadWave("sound/avoidance.mp3");
}

Vector3 Player::GetWorldPosition() { 
	//ワールド座標を入れる変数
	return Transform(Vector3{0, 0, 0}, worldTransform_.matWorld_);
}

Vector3 Player::GetWorldReticlePosition() { 
	// ワールド座標を入れる変数
	return Transform(Vector3{0, 0, 0}, worldTransform3DReticle_.matWorld_);
}

void Player::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
	worldTransform3DReticle_.parent_ = parent;
}

void Player::Update(const ViewProjection& viewProjection) {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	// Playerの移動処理と移動制限
	// 回避処理
	Avoidance();
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();

	// 2DReticleと3DReticleの座標計算
	ReticleUpdate(viewProjection);

	// ReticleとEnemyの当たり判定
	ReticleCollision(viewProjection);

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
	// ゲージバーの更新
	spriteGauge_->SetPosition(Vector2{160.0f * targetReticleTima_ + 48.0f, 68.0f});
	spriteGauge_->SetSize(Vector2{320.0f * targetReticleTima_, 48.0f});

	spriteHPGauge_->SetPosition(Vector2{16.0f * playerHP_ + 48.0f, 652.0f});
	spriteHPGauge_->SetSize(Vector2{32.0f * playerHP_, 48.0f});
	isDead_ = false;
	if (playerHP_ <= 0) {
		isDead_ = true;
	}
}

void Player::Avoidance() {
	XINPUT_STATE joyState;
	// 前フレームでのRBボタンの状態を記録
	static bool wasLBPressed = false;
	static bool wasRBPressed = false;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	// 現在のRBボタンの状態を取得
	bool isLBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
	bool isRBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

	if (!playerLeftRotate_ && !playerRightRotate_) {
		if (isLBPressed && !wasLBPressed) {
			playerLeftRotate_ = true;
			audio_->PlayWave(avoidanceSound_, false, 0.3f);
		}
		if (isRBPressed && !wasRBPressed) {
			playerRightRotate_ = true;
			audio_->PlayWave(avoidanceSound_, false, 0.3f);
		} 
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			worldTransform_.rotation_.x = (float)joyState.Gamepad.sThumbLY / SHRT_MAX * -0.1f;
			worldTransform_.rotation_.z = (float)joyState.Gamepad.sThumbLX / SHRT_MAX * -0.1f;
		}
	}
	wasLBPressed = isLBPressed;
	wasRBPressed = isRBPressed;
	if (playerLeftRotate_ && rotateLeftTime_ < 1.0f) {
		rotateLeftTime_ += 0.1f;
		worldTransform_.rotation_.z += 2.0f * pi * 0.1f;
		railCamera_->SetRotate(-0.01f);
	}
	if (playerRightRotate_ && rotateRightTime_ < 1.0f) {
		rotateRightTime_ += 0.1f;
		worldTransform_.rotation_.z -= 2.0f * pi * 0.1f;
		railCamera_->SetRotate(0.01f);
	}
	if (rotateLeftTime_ >= 1.0f || rotateRightTime_ >= 1.0f) {
		playerLeftRotate_ = false;
		rotateLeftTime_ = 0.0f;
		playerRightRotate_ = false;
		rotateRightTime_ = 0.0f;
		railCamera_->SetRotate(0.0f);
	}
}

void Player::ReticleUpdate(const ViewProjection& viewProjection) {
	// 2DReticleの移動処理
	spritePosition_ = sprite2DReticle_->GetPosition();

	// 3Dレティクルの位置計算
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
	// Matrix4x4 matInverseVPV = Inverse((viewProjection.matView * viewProjection.matProjection) * matViewport);
	Matrix4x4 matInverseVPV = Inverse(matViewport) * Inverse(viewProjection.matProjection);
	Vector3 posNear = Vector3(spritePosition_.x, spritePosition_.y, 0.0f);
	Vector3 posFar = Vector3(spritePosition_.x, spritePosition_.y, 1.0f);
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);
	Vector3 mouseDirection = Subtract(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	// カメラからの距離
	const float kDistanceTextObject = 100.0f;
	worldTransform3DReticle_.translation_ = Add(posNear, Multiply(kDistanceTextObject, mouseDirection));
	worldTransform3DReticle_.UpdateMatrix();

	targetReticleTima_ += 1.0f / 600.0f;
	if (targetReticleTima_ >= 1.0f) {
		targetReticleTima_ = 1.0f;
	}
}

void Player::ReticleCollision(const ViewProjection& viewProjection) {
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
	Matrix4x4 matViewProjectionViewport = viewProjection.matView * viewProjection.matProjection * matViewport;
	sprite2DReticle_->SetColor(Vector4{0.0f, 0.0f, 0.0f, 1.0f});

	// 敵ターゲットのフラグの初期化
	isTargetingEnemy_ = false;

	for (Enemy* enemy : enemys_) {
		// 敵のワールド座標を取得
		Vector3 positionEnemy = enemy->GetWorldPosition();
		// 敵をスクリーン座標に変換
		positionEnemy = Transform(positionEnemy, matViewProjectionViewport);
		// 敵が視界の外ならレティクルを外す
		Vector3 screenPositionEnemy = Transform(enemy->GetWorldPosition(), viewProjection.matView);
		// Z座標が0以下なら視界外と判定
		if (screenPositionEnemy.z <= 0.0f) {
			enemy->SetIsTargetingEnemy(false);
			enemy->SetHasPlayedReticleSound(false);
			continue;
		}
		// スクリーン座標の衝突判定
		bool isColliding = CheckCollisionCircleCircle(Vector3{spritePosition_.x, spritePosition_.y, 0.0f}, 16, positionEnemy, 16);
		if (isColliding && positionEnemy.z < 1.0f) {
			sprite2DReticle_->SetColor(Vector4{1.0f, 0.0f, 0.0f, 1.0f});
			if (targetReticleTima_ == 1.0f) {
				isTargetingEnemy_ = true;
				enemy->SetIsTargetingEnemy(isTargetingEnemy_);

				// まだこの敵に対して音が再生されていない場合のみ再生する
				if (!enemy->GetHasPlayedReticleSound()) {
					audio_->PlayWave(reticleSound_, false, 0.04f);
					enemy->SetHasPlayedReticleSound(true);
				}
			}
		}
		// セッターで入れた敵のフラグがtrueなら
		if (enemy->GetIsTargetingEnemy()) {
			isTargetingEnemy_ = enemy->GetIsTargetingEnemy();
		}
	}

	// キャラクターの攻撃処理
	if (isTargetingEnemy_) {
		// ターゲットしている場合はターゲットした敵に弾を発射
		TargetAttack();
	} else {
		// ターゲットしていない場合はレティクルに向けて弾を発射
		Attack(viewProjection);
	}
}

void Player::Attack(const ViewProjection& viewProjection) {
	XINPUT_STATE joyState;
	// 前フレームでのRBボタンの状態を記録
	static bool wasRBPressed = false;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	// 現在のRBボタンの状態を取得
	bool isRBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;

	if (isRBPressed && !wasRBPressed) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		Vector3 targetPosition = GetWorldReticlePosition();
		bool isColliding = false;
		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
		Matrix4x4 matViewProjectionViewport = viewProjection.matView * viewProjection.matProjection * matViewport;
		for (Enemy* enemy : enemys_) {
			// 敵のワールド座標を取得
			Vector3 positionEnemy = enemy->GetWorldPosition();
			// 敵をスクリーン座標に変換
			positionEnemy = Transform(positionEnemy, matViewProjectionViewport);
			// スクリーン座標の衝突判定
			isColliding = CheckCollisionCircleCircle(Vector3{spritePosition_.x, spritePosition_.y, 0.0f}, 16, positionEnemy, 16);
			if (isColliding && positionEnemy.z < 1.0f) {
				isColliding = true;
				targetPosition = enemy->GetWorldPosition();
				break;
			}
		}
		// 弾を生成し、初期化
		BulletInitialize(kBulletSpeed, targetPosition, velocity);
	}
	wasRBPressed = isRBPressed;
}

void Player::TargetAttack() {
	XINPUT_STATE joyState;
	// 前フレームでのRBボタンの状態を記録
	static bool wasRBPressed = false;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	// 現在のRBボタンの状態を取得
	bool isRBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;

	if (isRBPressed && !wasRBPressed) {
		for (Enemy* enemy : enemys_) {
			if (enemy->GetIsTargetingEnemy()) {
				// 敵のワールド座標を取得
				Vector3 enemyWorldPos = enemy->GetWorldPosition() + enemy->GetEnemySpeed() * 30.0f;

				// 弾を生成し、初期化
				PlayerBullet* newBullet = new PlayerBullet();
				newBullet->TargetInitialize(bulletModel_, GetWorldPosition(), enemyWorldPos);
				newBullet->SetRotate(railCamera_->GetWorldTransform().rotation_);
				// 弾を登録する
				bullets_.push_back(newBullet);

				enemy->SetIsTargetingEnemy(false);
				targetReticleTima_ = 0.0f;
				bulletAttackNum_++;
				audio_->PlayWave(bulletSound_, false, 0.01f);
			}
		}
	}
	wasRBPressed = isRBPressed;
}

void Player::BulletInitialize(float bulletSpeed, Vector3 worldPosition, Vector3 velocity) {

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = Subtract(worldPosition, GetWorldPosition());
	velocity = Multiply(bulletSpeed, Normalize(velocity));
	// 弾を生成し、初期化
	PlayerBullet* newBullet = new PlayerBullet();
	newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
	newBullet->SetRotate(railCamera_->GetWorldTransform().rotation_);

	// 弾を登録する
	bullets_.push_back(newBullet);
	bulletAttackNum_++;
	audio_->PlayWave(bulletSound_, false, 0.02f);
}

void Player::onCollision() {
	playerHP_--;
	audio_->PlayWave(playerDamage_, false, 0.1f);
	isDead_ = true;
}

float Player::GetRadius() { return 1.0f; }

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	playerModel_->Draw(worldTransform_, viewProjection);
	//playerModel_->Draw(worldTransform3DReticle_, viewProjection);
	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI() { 
	spriteGaugeFrame_->Draw();
	spriteGauge_->Draw();
	spriteHitu_->Draw();
	spriteHPGaugeFrame_->Draw();
	spriteHPGauge_->Draw();
	sprite2DReticle_->Draw();
}