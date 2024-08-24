#include "MiniMap.h"
#include <imgui.h>

MiniMap::~MiniMap() { delete sprite_; }

void MiniMap::Initialize(uint32_t textureHandle, const WorldTransform* world) {
	world_ = world;
	Vector2 spritePosition = {
	    world_->translation_.x / 50.0f * 128.0f + 1184.0f,
	    world_->translation_.z / 50.0f * -128.0f + 96.0f,
	};
	sprite_ = Sprite::Create(textureHandle, spritePosition, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	sprite_->SetSize(Vector2{20, 20});
	isDraw = true;
}

void MiniMap::Update() {
	Vector2 spritePosition = {
	    world_->translation_.x / 200.0f * 128 + 1184.0f,
	    world_->translation_.z / 200.0f * -128 + 96.0f,
	};
	sprite_->SetPosition(spritePosition);
	ImGui::Text("x,%f : y,%f", spritePosition.x, spritePosition.y);
	ImGui::Text("%d", isDraw);

	if (sprite_->GetPosition().y > 1000000) {
		isDraw = false;
	}
}

void MiniMap::DrawUI() {
	if (isDraw) {
		sprite_->Draw();
	}
}
