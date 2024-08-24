#pragma once
#include "list"
#include <WorldTransform.h>
#include <ViewProjection.h>
#include <WinApp.h>
#include <Sprite.h>
#include "MT3.h"

class MiniMap {
public:
	~MiniMap();
	void Initialize(uint32_t textureHandle, const WorldTransform* world);
	bool IsDraw() { return isDraw; }

	void Update();

	void DrawUI();

private:
	const WorldTransform* world_;
	Sprite* sprite_;
	bool isDraw = false;
};
