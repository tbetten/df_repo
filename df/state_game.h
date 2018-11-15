#pragma once
#include "state.h"
#include "eventmanager.h"
#include "statemanager.h"
#include "tileset.h"
#include "tilemap.h"

#include <SFML\Graphics.hpp>
#include <memory>

class CMD_game_menu : public Command
{
public:
	CMD_game_menu(std::string name, Actor* actor) : Command{ name, actor } {}
	void execute() override;
};

class CMD_pause : public Command
{
public:
	CMD_pause(std::string name, Actor* actor) : Command{ name, actor } {}
	void execute() override;
};

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

	void to_mainmenu(const Event_details& details);
	void pause(const Event_details& details);

private:
	Layer m_l;
	sf::Sprite m_sprite;
	sf::Sprite m_sprite2;
	sf::Texture m_text;
	sf::VertexArray m_vert;
	Tileset m_tileset;
	//Grid m_grid;
};