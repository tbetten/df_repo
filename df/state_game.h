#pragma once
#include "state.h"
#include "eventmanager.h"
#include "statemanager.h"
#include "tileset.h"
#include "tilemap.h"
#include "hexlib.h"

#include <SFML\Graphics.hpp>
#include <memory>

/*
class CMD_game_menu : public Command
{
public:
	CMD_game_menu(std::string name, Actor* actor) : Command{ name, actor } {}
	void execute() override;
};*/
/*
class CMD_pause : public Command
{
public:
	CMD_pause(std::string name, Actor* actor) : Command{ name, actor } {}
	void execute() override;
};*/

class State_game : public State
{
public:
	State_game(Shared_context *context) : State{ context } {}

	void on_create() override;
	void on_destroy() override;
	void activate() override {}
	void deactivate() override {}
	void update(const sf::Time& time) override;
	void draw() override;

	void to_mainmenu();
	void pause();

private:
	sf::VertexArray m_grid;
	hexlib::Layout m_grid_layout;
	bool m_first{ true };
};