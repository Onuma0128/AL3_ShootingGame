#include "TitleScene.h"
#include "TextureManager.h"
#include <algorithm> 
#include "imgui.h"

TitleScene::~TitleScene() {
	delete titleSprite_;
	delete sousaSprite_;
}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();
	isBbutton_ = false;
	uint32_t titleTexture = TextureManager::Load("Title.png");
	uint32_t sousaTexture = TextureManager::Load("sousa.png");
	titleSprite_ = Sprite::Create(titleTexture, Vector2{640.0f, 360.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	sousaSprite_ = Sprite::Create(sousaTexture, Vector2{640.0f, 400.0f}, Vector4{1.0f, 1.0f, 1.0f, 1.0f}, Vector2{0.5f, 0.5f});
	sousaSprite_->SetSize(Vector2{0, 0});
	clickSound_ = audio_->LoadWave("sound/click.wav");
}

void TitleScene::Update() {
	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B && !isBbutton_) {
		isBbutton_ = true;
		audio_->PlayWave(clickSound_, false, 0.1f);
	}
	if (isBbutton_ && !isSpriteSmall_ && spriteSize_.x < 464.0f) {
		spriteSize_.x += 464.0f / 60.0f;
		spriteSize_.y += 272.0f / 60.0f;
		sousaSprite_->SetSize(spriteSize_);
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B && spriteSize_.x == 464.0f) {
		isSpriteSmall_ = true;
		audio_->PlayWave(clickSound_, false, 0.1f);
	}
	if (isSpriteSmall_ && spriteSize_.x > 0.0f) {
		spriteSize_.x -= 464.0f / 60.0f;
		spriteSize_.y -= 272.0f / 60.0f;
		sousaSprite_->SetSize(spriteSize_);
	}
	if (spriteSize_.x == 0.0f) {
		isBbutton_ = false;
		isSpriteSmall_ = false;
	}

	static bool wasRBPressed = false;
	bool isRBPressed = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;

	if (isRBPressed && !wasRBPressed && !isBbutton_) {
		finished_ = true;
		audio_->PlayWave(clickSound_, false, 0.1f);
	}
	wasRBPressed = isRBPressed;

	spriteSize_.x = std::clamp(spriteSize_.x, 0.0f, 464.0f);
	spriteSize_.y = std::clamp(spriteSize_.y, 0.0f, 272.0f);
}

void TitleScene::Draw() {
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
	titleSprite_->Draw();
	sousaSprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
