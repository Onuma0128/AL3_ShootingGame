#pragma once
#include <string>
#include "Windows.h"

class Enemy;

class BaseEnemyState {
public:

	virtual ~BaseEnemyState() = default;
	BaseEnemyState(const std::string& name, Enemy* enemy) : name_(name), enemy_(enemy) {};

	// 毎フレーム処理
	virtual void Update() = 0;

	// デバッグログ出力
	virtual void DebugLog();

	// ログ
	void Log(const std::string& message);

protected:
	// 状態名
	std::string name_;
	// 操作対象の敵
	Enemy* enemy_ = nullptr;
};