#include "Laser.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Laser::Laser(Game* game)
	: Actor(game)
	, mDeathTimer(1.0f)
{
	// create sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Laser.png"));

	// create a move component, and set a forward speed
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(800.0f);

	// create a circle component (for collision)
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(11.0f);
}

void Laser::UpdateActor(float deltaTime)
{
	// if we run out of time, laser is dead
	mDeathTimer -= deltaTime;

	if (mDeathTimer <= 0.0f)
	{
		SetState(EDead);
	}
	else
	{
		// do we intersect with an asteroid?
		for (auto ast : GetGame()->GetAsteroids())
		{
			if (Intersect(*mCircle, *(ast->GetCircle())))
			{
				// the first asteroid we intersect with,
				// set ourselves and the asteroid dead
				SetState(EDead);
				ast->SetState(EDead);
				break;
			}
		}
	}
}
