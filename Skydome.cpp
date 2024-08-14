#include "Skydome.h"
#include <cassert>
#include "MT3.h"

void Skydome::Initialize(Model* model) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.scale_ = {300, 300, 300};
}

void Skydome::Update() {
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection);
}