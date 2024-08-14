#pragma once
#include "MT3.h"

class Collider {
public:
	// 半径を取得
	virtual float GetRadius() { return radius_; }
	// 半径を設定
	void SetRadius(float radius) { radius_ = radius; }
	// 衝突時に呼ばれる関数
	virtual void onCollision() = 0;
	// ワールド座標を取得
	virtual Vector3 GetWorldPosition() = 0;

private:
	float radius_ = 1.0f;
};