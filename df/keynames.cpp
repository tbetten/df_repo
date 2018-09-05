#include "stdafx.h"
#include "keynames.h"

//Keynames::Keynames() : keycodes{ sf::Keyboard::KeyCount, 1 } {}

String_bimap <sf::Keyboard::Key> Keynames::keycodes{ sf::Keyboard::KeyCount, 1 };

const std::string& Keynames::to_string(sf::Keyboard::Key keycode)
{
	if (keycodes.size() == 0)
	{
		init();
	}
	return keycodes.at(keycode);
}

const sf::Keyboard::Key Keynames::to_keycode(const std::string& keyname)
{
	if (keycodes.size() == 0)
	{
		init();
	}
	return keycodes.at(keyname);
}

void Keynames::init()
{
	keycodes.insert(sf::Keyboard::Key::Unknown, "Unknown");
	keycodes.insert(sf::Keyboard::Key::A, "A");
	keycodes.insert(sf::Keyboard::Key::B, "B");
	keycodes.insert(sf::Keyboard::Key::C, "B");
	keycodes.insert(sf::Keyboard::Key::D, "D");
	keycodes.insert(sf::Keyboard::Key::E, "E");
	keycodes.insert(sf::Keyboard::Key::F, "F");
	keycodes.insert(sf::Keyboard::Key::G, "G");
	keycodes.insert(sf::Keyboard::Key::H, "H");
	keycodes.insert(sf::Keyboard::Key::I, "I");
	keycodes.insert(sf::Keyboard::Key::J, "J");
	keycodes.insert(sf::Keyboard::Key::K, "K");
	keycodes.insert(sf::Keyboard::Key::L, "L");
	keycodes.insert(sf::Keyboard::Key::M, "M");
	keycodes.insert(sf::Keyboard::Key::N, "N");
	keycodes.insert(sf::Keyboard::Key::O, "O");
	keycodes.insert(sf::Keyboard::Key::P, "P");
	keycodes.insert(sf::Keyboard::Key::Q, "Q");
	keycodes.insert(sf::Keyboard::Key::R, "R");
	keycodes.insert(sf::Keyboard::Key::S, "S");
	keycodes.insert(sf::Keyboard::Key::T, "T");
	keycodes.insert(sf::Keyboard::Key::U, "U");
	keycodes.insert(sf::Keyboard::Key::V, "V");
	keycodes.insert(sf::Keyboard::Key::W, "W");
	keycodes.insert(sf::Keyboard::Key::X, "X");
	keycodes.insert(sf::Keyboard::Key::Y, "Y");
	keycodes.insert(sf::Keyboard::Key::Z, "Z");

	keycodes.insert(sf::Keyboard::Key::Escape, "Escape");

	keycodes.insert(sf::Keyboard::Key::Space, "Space");

	keycodes.insert(sf::Keyboard::Key::Left, "Left");
	keycodes.insert(sf::Keyboard::Key::Right, "Right");
	keycodes.insert(sf::Keyboard::Key::Up, "Up");
	keycodes.insert(sf::Keyboard::Key::Down, "Down");

	keycodes.insert(sf::Keyboard::Key::F1, "F1");
	keycodes.insert(sf::Keyboard::Key::F2, "F2");
	keycodes.insert(sf::Keyboard::Key::F3, "F3");
	keycodes.insert(sf::Keyboard::Key::F4, "F4");
	keycodes.insert(sf::Keyboard::Key::F5, "F5");
	keycodes.insert(sf::Keyboard::Key::F6, "F6");
	keycodes.insert(sf::Keyboard::Key::F7, "F7");
	keycodes.insert(sf::Keyboard::Key::F8, "F8");
	keycodes.insert(sf::Keyboard::Key::F9, "F9");
	keycodes.insert(sf::Keyboard::Key::F10, "F10");
	keycodes.insert(sf::Keyboard::Key::F11, "F11");
	keycodes.insert(sf::Keyboard::Key::F12, "F12");
	keycodes.insert(sf::Keyboard::Key::F13, "F13");
	keycodes.insert(sf::Keyboard::Key::F14, "F14");
	keycodes.insert(sf::Keyboard::Key::F15, "F15");
}