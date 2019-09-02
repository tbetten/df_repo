#pragma once
#include "state.h"
#include "resource_cache.h"

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
	Eventmanager* m_event_mgr = nullptr;
//	std::shared_ptr<cache::Resource_base> m_font_resource;
//	sf::Font* m_font;
//	sf::Text m_text;
};