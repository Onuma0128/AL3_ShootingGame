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
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	void SetTarget(const Vector3 target);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	Vector3 target_{};
	//ビュープロジェクション
	ViewProjection viewProjection_;

	Input* input_ = nullptr;
};
