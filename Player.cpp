#include "Player.h"
#include "ImGuiManager.h"
#include <cassert>
#include "Enemy.h"
#include "RailCamera.h"

Player::~Player() { 
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, uint32_t textureHandle, Vector3 playerPos) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.translation_ = playerPos;
	worldTransform_.Initialize();
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	//3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();
	//レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("2dReticle.png");
	//スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, Vector2{640.0f, 360.0f}, Vector4{0.0f, 0.0f, 0.0f, 1.0f}, Vector2{0.5f, 0.5f});
	targetReticleTima_ = 0.0f;
	isTargetingEnemy_ = false;
}

Vector3 Player::GetWorldPosition() { 
	//ワールド座標を入れる変数
	return Transform(Vector3{0, 0, 0}, worldTransform_.matWorld_);
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
	PlayerMove();

	// 2DReticleと3DReticleの座標計算
	ReticleUpdate(viewProjection);

	// ReticleとEnemyの当たり判定
	ReticleCollision(viewProjection);

	// PlayerParameterのImGui
	PlayerParameter();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::PlayerMove() {
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};
	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;
	// ジョイスティックの入力処理
	XINPUT_STATE joystate;
	if (Input::GetInstance()->GetJoystickState(0, joystate)) {
		move.x += (float)joystate.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joystate.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}
	// 移動限界座標
	const float kMoveLimiX = 33.0f;
	const float kMoveLimiY = 18.0f;
	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimiX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimiX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimiY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimiY);

	// 座標移動(ベクトルの加算)
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();
}

void Player::ReticleUpdate(const ViewProjection& viewProjection) {
	// 2DReticleの移動処理
	spritePosition_ = sprite2DReticle_->GetPosition();
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition_.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 10.0f;
		spritePosition_.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 10.0f;

		sprite2DReticle_->SetPosition(spritePosition_);
	}

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

	targetReticleTima_--;
	if (targetReticleTima_ <= 0.0f) {
		targetReticleTima_ = 0.0f;
	}
}

void Player::ReticleCollision(const ViewProjection& viewProjection) {
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
	Matrix4x4 matViewProjectionViewport = viewProjection.matView * viewProjection.matProjection * matViewport;

	// 敵ターゲットのフラグの初期化
	isTargetingEnemy_ = false;

	for (Enemy* enemy : enemys_) {
		// 敵のワールド座標を取得
		Vector3 positionEnemy = enemy->GetWorldPosition();
		// 敵をスクリーン座標に変換
		positionEnemy = Transform(positionEnemy, matViewProjectionViewport);
		// スクリーン座標の衝突判定
		bool isColliding = CheckCollisionCircleCircle(Vector3{spritePosition_.x, spritePosition_.y, 0.0f}, 16, positionEnemy, 16);
		if (isColliding) {
			worldTransform3DReticle_.translation_ = enemy->GetWorldPosition();
			if (targetReticleTima_ == 0.0f) {
				isTargetingEnemy_ = true;
				enemy->SetIsTargetingEnemy(isTargetingEnemy_);
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
		Attack();
	}
}

void Player::PlayerParameter() {
	// PlayerParameter
	ImGui::Begin("GameParameter");
	// Player
	ImGui::Text("Player");
	ImGui::Text("x:%f,y:%f,z:%f",
		worldTransform_.translation_.x,
		worldTransform_.translation_.y,
		worldTransform_.translation_.z
	);

	ImGui::Text("targetReticleTima\n%f", targetReticleTima_);
	// PlayerReticle
	ImGui::Text("Reticle3D");
	ImGui::Text("x:%f,y:%f,z:%f", 
		worldTransform3DReticle_.translation_.x, 
		worldTransform3DReticle_.translation_.y, 
		worldTransform3DReticle_.translation_.z
	);
	ImGui::End();
}

void Player::Attack() {
	XINPUT_STATE joyState;
	// 前フレームでのRBボタンの状態を記録
	static bool wasRBPressed = false;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	// 現在のRBボタンの状態を取得
	bool isRBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

	if (isRBPressed && !wasRBPressed) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 弾を生成し、初期化
		BulletInitialize(kBulletSpeed, worldTransform3DReticle_.translation_, velocity);
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
	bool isRBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

	if (isRBPressed && !wasRBPressed) {
		for (Enemy* enemy : enemys_) {
			if (enemy->GetIsTargetingEnemy()) {
				// 弾の速度
				const float kBulletSpeed = 2.0f;
				Vector3 velocity(0, 0, kBulletSpeed);
				// 敵のワールド座標を取得
				Vector3 enemyWorldPos = enemy->GetWorldPosition();
				// ビュー行列で敵の座標を変換
				Matrix4x4 viewMatrix = railCamera_->GetViewProjection().matView;
				Vector3 enemyCameraPos = Transform(enemyWorldPos, viewMatrix);	
				// 弾を生成し、初期化
				BulletInitialize(kBulletSpeed, enemyCameraPos, velocity);
				enemy->SetIsTargetingEnemy(false);
				targetReticleTima_ = 300.0f;
			}
		}
	}
	wasRBPressed = isRBPressed;
}

void Player::BulletInitialize(float bulletSpeed, Vector3 worldPosition, Vector3 velocity) {

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = Subtract(worldPosition, worldTransform_.translation_);
	velocity = Multiply(bulletSpeed, Normalize(velocity));
	// 弾を生成し、初期化
	PlayerBullet* newBullet = new PlayerBullet();
	newBullet->SetParent(worldTransform_.parent_);
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);
}

void Player::onCollision() {}

float Player::GetRadius() { return 1.0f; }

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	/*model_->Draw(worldTransform3DReticle_, viewProjection, textureHandle_);*/
	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI() { 
	sprite2DReticle_->Draw();
}