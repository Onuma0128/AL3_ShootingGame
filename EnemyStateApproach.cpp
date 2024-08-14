#include "EnemyStateApproach.h"
#include "Enemy.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy)
	:BaseEnemyState("State Approach", enemy) {
}

void EnemyStateApproach::Update() {
	Vector3 move{ 0.0f, 0.0f, -0.1f };
	enemy_->EnemyMove(move);
}