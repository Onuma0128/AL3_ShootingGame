#include "GameOver.h"
#include <algorithm> 

GameOver::~GameOver() {
	delete spriteScoreScene_;
	for (uint32_t i = 0; i < 50; ++i) {
		delete spriteNumber_[i];
	}
}

void GameOver::Initialize() { 
	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();
	gameScore_ = 0;
	gameScoreCount_ = 0;
	uint32_t scoreSceneTexture = TextureManager::Load("GameScore.png");
	spriteScoreScene_ = Sprite::Create(scoreSceneTexture, Vector2{640.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	uint32_t numberTexture[10] = { 
		TextureManager::Load("number/0.png"),
		TextureManager::Load("number/1.png"),
		TextureManager::Load("number/2.png"),
		TextureManager::Load("number/3.png"),
		TextureManager::Load("number/4.png"),
		TextureManager::Load("number/5.png"),
		TextureManager::Load("number/6.png"),
		TextureManager::Load("number/7.png"),
		TextureManager::Load("number/8.png"),
		TextureManager::Load("number/9.png")
	};
	for (uint32_t i = 0; i < 10; ++i) {
		spriteNumber_[i] = Sprite::Create(numberTexture[i], Vector2{800.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
		spriteNumber_[i + 10] = Sprite::Create(numberTexture[i], Vector2{720.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
		spriteNumber_[i + 20] = Sprite::Create(numberTexture[i], Vector2{640.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
		spriteNumber_[i + 30] = Sprite::Create(numberTexture[i], Vector2{560.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
		spriteNumber_[i + 40] = Sprite::Create(numberTexture[i], Vector2{480.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	}
	clickSound_ = audio_->LoadWave("sound/click.wav");
	ScoreCountSound_ = audio_->LoadWave("sound/scoreCount.mp3");
}

void GameOver::Update() {
	gameScore_ = std::clamp(gameScore_, 0.0f, 99999.0f);
	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		audio_->PlayWave(clickSound_, false, 0.1f);
		audio_->StopWave(voiceHandle_);
		finished_ = true;
	}
	if (gameScore_ != gameScoreCount_) {
		gameScoreCount_ += gameScore_ / 120.0f;
		audio_->PlayWave(ScoreCountSound_, false, 0.05f);
	}
	gameScoreCount_ = std::clamp(gameScoreCount_, 0.0f, gameScore_);
	int score = static_cast<int>(gameScoreCount_);
	digit_[0] = score % 10;           // 一の位
	digit_[1] = (score / 10) % 10;    // 十の位
	digit_[2] = (score / 100) % 10;   // 百の位
	digit_[3] = (score / 1000) % 10;  // 千の位
	digit_[4] = (score / 10000) % 10; // 万の位
}

void GameOver::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	spriteScoreScene_->Draw();
	spriteNumber_[digit_[4] + 40]->Draw(); // 万の位
	spriteNumber_[digit_[3] + 30]->Draw(); // 千の位
	spriteNumber_[digit_[2] + 20]->Draw(); // 百の位
	spriteNumber_[digit_[1] + 10]->Draw(); // 十の位
	spriteNumber_[digit_[0]]->Draw();      // 一の位

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}