#pragma once
#include <Input.h>

class TitleScene {
public:
	void Initialize();
	bool IsFinished() const { return finished_; }

	void Update();

	void Draw();

private:
	// 終了フラグ
	bool finished_ = false;
};
