#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent
{
public:
	// lower update order to update first
	InputComponent(class Actor* owner);

	void ProcessInput(const uint8_t* keyState) override;

	// getters / setters
	float GetMaxForward() const { return mMaxForwardSpeed; }
	float GetMaxAngular() const { return mMaxAngularSpeed; }
	int GetForwardKey() const { return mForwardKey; }
	int GetBackKey() const { return mBackKey; }
	int GetClockwiseKey() const { return mClockwiseKey; }
	int GetCounterClockwiseKey() const { return mCounterClockwiseKey; }

	void SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
	void SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed; }
	void SetForwardKey(int key) { mForwardKey = key; }
	void SetBackKey(int key) { mBackKey = key; }
	void SetClockwiseKey(int key) { mClockwiseKey = key; }
	void SetCounterClockwiseKey(int key) { mCounterClockwiseKey = key };

private:
	// the max forward/angular speeds
	float mMaxForwardSpeed;
	float mMaxAngularSpeed;

	// keys for forward/back movement
	int mForwardKey;
	int mBackKey;

	// keys for angular movement
	int mClockwiseKey;
	int mCounterClockwiseKey;
};

