#pragma once
#include "BaseEnemyState.h"

class EnemyStateLeave : public BaseEnemyState {
public:
	// コンストラクタ
	EnemyStateLeave(Enemy* enemy);
	// 更新
	void Update() override;
};