#pragma once

#include "utils.h"
#include <SFML\Window.hpp>

class Event_types
{
public:
	static const std::string& to_string(sf::Event::EventType eventcode);
	static const sf::Event::EventType to_eventcode(const std::string& eventname);
private:
	static void init();
	static String_bimap <sf::Event::EventType> eventcodes;
};