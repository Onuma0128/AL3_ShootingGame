#include "EnemyStateLeave.h"
#include "Enemy.h"

EnemyStateLeave::EnemyStateLeave(Enemy* enemy)
	:BaseEnemyState("State Leave", enemy) {
}

void EnemyStateLeave::Update() {
	Vector3 move{ -0.1f, 0.1f, 0.0f };
	enemy_->EnemyMove(move);
}