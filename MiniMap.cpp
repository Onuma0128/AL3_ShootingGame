#include "MiniMap.h"

MiniMap::~MiniMap() { delete sprite_; }

void MiniMap::Initialize(uint32_t textureHandle, const WorldTransform* world) {
	world_ = world;
	Vector2 spritePosition = {
	    world_->translation_.x / 200.0f * 96.0f + 1184.0f,
	    world_->translation_.z / 200.0f * -96.0f + 96.0f,
	};
	sprite_ = Sprite::Create(textureHandle, spritePosition, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	sprite_->SetSize(Vector2{20, 20});
	isDraw = true;
}

void MiniMap::Update() {
	Vector2 spritePosition = {
	    world_->translation_.x / 200.0f * 96.0f + 1184.0f,
	    world_->translation_.z / 200.0f * -96.0f + 96.0f,
	};

	sprite_->SetRotation(world_->rotation_.y);
	sprite_->SetPosition(spritePosition);

	if (world_->scale_.x == 0) {
		isDraw = false;
	}
}

void MiniMap::DrawUI() {
	if (isDraw) {
		sprite_->Draw();
	}
}
