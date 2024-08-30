#pragma once
#include "DirectXCommon.h"
#include "Audio.h"
#include "Model.h"
#include "Sprite.h"
#include "Input.h"
#include "TextureManager.h"

class GameOver {
public:
	~GameOver();

	void Initialize();
	bool IsFinished() const { return finished_; }
	void SetGameScore(const float& gameScore) { gameScore_ = gameScore; }
	void SetGameSceneBGM(const uint32_t& voiceHandle) { voiceHandle_ = voiceHandle; }

	void Update();

	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;
	// ゲームシーンのBGM
	uint32_t voiceHandle_ = 0;
	// 番号のスプライト
	Sprite* spriteNumber_[50]{};
	Sprite* spriteScoreScene_{};
	uint32_t clickSound_ = 0;
	uint32_t ScoreCountSound_ = 0;
	// ゲームのスコア
	float gameScore_ = 0;
	float gameScoreCount_ = 0;
	int digit_[5] = {};
	// 終了フラグ
	bool finished_ = false;
};
