#include "Game.h"
#include "SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"
#include "BGSpriteComponent.h"

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mActors()
	, mPendingActors()
	, mUpdatingActors(false)
{
}

bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// create an sdl window
	mWindow = SDL_CreateWindow(
		"SDL Game",
		100,
		100,
		1024,
		768,
		0
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_Image: %s", SDL_GetError());
		return false;
	}

	LoadData();

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// if updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);

	if (iter != mPendingActors.end())
	{
		// swap to end of vector and pop off
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);

	if (iter != mActors.end())
	{
		// swap to end and pop off
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// find the insertion point in the sorted vector
	// (the first element with a higher draw order)
	int myDrawOrder = sprite->GetDrawOrder();

	auto iter = mSprites.begin();

	for (; iter != mSprites.end(); ++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// insert element before position of iterator
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	// is the texture already in the map?
	auto iter = mTextures.find(fileName);

	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// load from file
		SDL_Surface* surf = IMG_Load(fileName.c_str());

		if (!surf)
		{
			SDL_Log("Failed to load texture file: %s", fileName.c_str());
			return nullptr;
		}

		// create texture from surface
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);

		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}
	}

	return tex;
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), ast);

	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}

void Game::ProcessInput()
{
	SDL_Event event;

	// while there are still events in the queue
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// process ship input
	mShip->ProcessKeyboard(state);
}

void Game::UpdateGame()
{
	// wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// clamp maximum delta time value
	if (deltaTime > 0.0f)
	{
		deltaTime = 0.05f;
	}

	mTicksCount = SDL_GetTicks();

	// update all actors
	mUpdatingActors = true;
	
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}

	mUpdatingActors = false;

	// move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}

	mPendingActors.clear();

	// add any dead actors to a temp vector
	std::vector<Actor*> deadActors;

	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// delete dead actors
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// draw all sprite components
	if (mSprites.size() > 0)
	{
		for (auto sprite : mSprites)
		{
			sprite->Draw(mRenderer);
		}
	}

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::LoadData()
{
	// create the players ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(100.0f, 384.0f));
	mShip->SetScale(1.5f);

	// create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));

	// create the far back background
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	std::vector<SDL_Texture*> bgTexs = {
		GetTexture("Assets/Farback01.png"),
		GetTexture("Assets/Farback02.png")
	};
	bg->SetBGTextures(bgTexs);
	bg->SetScrollSpeed(-100.0f);

	// create the closer background
	bg = new BGSpriteComponent(temp, 50);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	bgTexs = {
		GetTexture("Assets/Stars.png"),
		GetTexture("Assets/Stars.png")
	};
	bg->SetBGTextures(bgTexs);
	bg->SetScrollSpeed(-200.0f);

	// create asteroids
	const int numAsteroids = 20;

	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}
}

void Game::UnloadData()
{
	// delete actors
	if (mActors.size() > 0)
	{
		while (!mActors.back())
		{
			delete mActors.back();
		}
	}

	// destory textures
	if (mTextures.size() > 0)
	{
		for (auto i : mTextures)
		{
			SDL_DestroyTexture(i.second);
		}

		mTextures.clear();
	}
}
