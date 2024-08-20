#include "Particle.h"
#include <imgui.h>
#include <algorithm>

void Particle::Initialize(Model* model, uint32_t textureHandle, Vector3 Position, uint32_t particleNum, float particleTime) {
	model_ = model;
	textureHandle_ = textureHandle;
	particles_.resize(particleNum);
	for (auto& particle : particles_) {
		particle.worldTransform.translation_ = Position;
		float particleSize = (float)(rand() % 6 + 5) * 0.1f;
		particle.worldTransform.scale_ = {particleSize, particleSize, particleSize};
		particle.velocity = {(float)(rand() % 11 - 5) * 0.01f, (float)(rand() % 10 + 1) * 0.01f, 0.0f};
		particle.worldTransform.Initialize();
	}
	particleTime_ = 0;
	particleTimeCount_ = particleTime;
	particleColor_.Initialize();
	color_ = {1, 1, 1, 1};
	isActive_ = true;
}

void Particle::Update() {
	if (isActive_) {
		for (auto& particle : particles_) {
			particle.worldTransform.translation_ = particle.worldTransform.translation_ + particle.velocity;
			particle.worldTransform.UpdateMatrix();
		}
		particleTime_ += 1.0f / particleTimeCount_;
		color_.w -= 1.0f / particleTimeCount_;
		color_.w = std::clamp(color_.w, 0.0f, 1.0f);
		particleColor_.SetColor(color_);
		particleColor_.TransferMatrix();
		if (particleTime_ >= 1.0f) {
			isActive_ = false;
		}
	}
}

void Particle::Draw(ViewProjection& viewProjection) {
	for (const auto& particle : particles_) {
		if (isActive_) {
			// パーティクルの描画処理
			model_->Draw(particle.worldTransform, viewProjection, textureHandle_, &particleColor_);
		}
	}
}
