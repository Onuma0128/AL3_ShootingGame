#pragma once
#include <WorldTransform.h>
#include <ViewProjection.h>
#include <Model.h>
#include <list>
#include "MT3.h"

struct ParticleInstance {
	WorldTransform worldTransform;
	Vector3 velocity;
	float particleTime;
};

class Particle {
public:
	
	void Initialize(Model* model, uint32_t textureHandle, Vector3 Position, 
		uint32_t particleNum, float particleTime);
	bool IsActive() const { return isActive_; }

	void Update();

	void Draw(ViewProjection& viewProjection);

private:

	std::list<ParticleInstance> particles_;
	float particleTimeCount_ = 0;
	bool isActive_;
	ObjectColor particleColor_;
	Vector4 color_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
};
