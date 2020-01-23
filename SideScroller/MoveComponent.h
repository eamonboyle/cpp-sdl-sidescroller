#pragma once
#include "Component.h"

class MoveComponent : public Component
{
public:
	// lower update order to update first
	MoveComponent(class Actor* owner, int updateOrder = 10);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }

	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }

private:
	// controls rotation (radians/second)
	float mAngularSpeed;
	// controls forward movement (units/second)
	float mForwardSpeed;
};

