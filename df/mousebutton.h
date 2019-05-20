#pragma once

#include "utils.h"
#include <SFML\Window.hpp>

class Mousebutton
{
public:
	static const std::string& to_string(sf::Mouse::Button mbutton);
	static const sf::Mouse::Button to_buttoncode(const std::string& buttonname);
	static bool contains(const std::string& buttonname);
private:
	static void init();
	static String_bimap <sf::Mouse::Button> buttoncodes;
};