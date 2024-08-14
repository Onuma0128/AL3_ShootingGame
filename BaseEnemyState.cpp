#include "BaseEnemyState.h"

void BaseEnemyState::DebugLog() { Log(name_); }

void BaseEnemyState::Log(const std::string& message) {
    OutputDebugStringA(message.c_str());
}