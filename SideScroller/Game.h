#pragma once
#include <SDL.h>

#include <string>
#include <vector>

#undef main

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	SDL_Texture* GetTexture(const std::string& fileName);

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;

	bool mIsRunning;
	bool mUpdatingActors;
};

