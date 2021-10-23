#pragma once
#include <variant>
#include <string>
#include <SFML/Window.hpp>

struct Cursor_payload
{
	explicit Cursor_payload (sf::Cursor::Type t) : m_payload { t } {}
	explicit Cursor_payload (std::string s) : m_payload { s } {}

	std::variant<sf::Cursor::Type, std::string> m_payload;
};

