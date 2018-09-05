#pragma once
enum class Game_state { All_states, Intro, Main_menu, Game, Paused };

inline int state_to_int(Game_state state)
{
	return static_cast<int>(state);
}