#pragma once
#include "BaseEnemyState.h"

class EnemyStateApproach : public BaseEnemyState {
public:
	// コンストラクタ
	EnemyStateApproach(Enemy* enemy);

	// 更新
	void Update() override;

};