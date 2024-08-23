#include "EnemyStateApproach.h"
#include "Enemy.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy)
	:BaseEnemyState("State Approach", enemy) {
}

void EnemyStateApproach::Update() {
	Vector3 move{};
	move = enemy_->GetEnemySpeed();
	enemy_->EnemyMove(move);
}