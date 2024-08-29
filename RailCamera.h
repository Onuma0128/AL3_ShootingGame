#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"

class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	void PlayerDamageInitialze();
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	void SetTarget(const Vector3 target);
	void SetRotate(const float rotate);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	void PlayerDamageUpdate();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	Vector3 target_{};
	float rotate_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

	Input* input_ = nullptr;

	// プレイヤーがダメージを受けた時用
	Vector3 cameraDefaultPos_{};
	float randTime_;
	bool isPlayerDamage_ = false;
};
