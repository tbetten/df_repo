#include "stdafx.h"
#include "event_types.h"

String_bimap <sf::Event::EventType> Event_types::eventcodes{ sf::Event::EventType::Count };

const std::string& Event_types::to_string(sf::Event::EventType eventcode)
{
	if (eventcodes.size() == 0)
	{
		init();
	}
	return eventcodes.at(eventcode);
}

const sf::Event::EventType Event_types::to_eventcode(const std::string& eventname)
{
	if (eventcodes.size() == 0)
	{
		init();
	}
	return eventcodes.at(eventname);
}

void Event_types::init()
{
	eventcodes.insert(sf::Event::EventType::Closed, "Closed");

	eventcodes.insert(sf::Event::EventType::KeyPressed, "KeyPressed");

	eventcodes.insert(sf::Event::EventType::MouseButtonPressed, "MouseButtonPressed");
}
