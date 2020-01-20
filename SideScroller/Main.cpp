// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "Game.h"

int main()
{
	Game game;

	bool success = game.Initialize();

	if (success)
	{
		game.RunLoop();
	}

	game.Shutdown();

	return 0;
}