// df.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "game.h"


int main()
{
	Game game;
	while (!game.get_window()->is_done())
	{
		//game.handle_input();
		game.update();
		game.render();
		game.late_update();
		game.restart_clock();
	}
	std::cout << "bla";
	return 0;
}
