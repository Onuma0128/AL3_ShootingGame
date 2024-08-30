#pragma once
#include "DirectXCommon.h"
#include "Audio.h"
#include "Model.h"
#include "Sprite.h"
#include <Input.h>

class TitleScene {
public:
	~TitleScene();

	void Initialize();
	bool IsFinished() const { return finished_; }

	void Update();

	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;

	Sprite* titleSprite_{};
	Sprite* sousaSprite_{};
	uint32_t clickSound_ = 0;
	Vector2 spriteSize_ = {};
	bool isBbutton_ = false;
	bool isSpriteSmall_ = false;

	// 終了フラグ
	bool finished_ = false;
};
