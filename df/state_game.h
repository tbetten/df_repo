#pragma once
#include "state.h"

#include <memory>

struct Tilemap;

class State_game : public State
{
public:
	State_game(Shared_context* context) : State{ context } {}

	void on_create() override;
	void on_destroy() override;
	void activate() override {}
	void deactivate() override {}
	void update(const sf::Time& time) override;
	void draw() override;

	void to_mainmenu();
	void pause();
	void to_chargen();

private:
	bool m_first{ true };
	std::shared_ptr<Tilemap> m_map;
};