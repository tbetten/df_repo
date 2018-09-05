#pragma once
#include "utils.h"
#include <SFML\Window.hpp>

class Keynames
{
public:
	static const std::string& to_string(sf::Keyboard::Key keycode);
	static const sf::Keyboard::Key to_keycode(const std::string& keyname);
private:
	static void init();
	static String_bimap <sf::Keyboard::Key> keycodes;
};