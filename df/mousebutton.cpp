#include "stdafx.h"
#include "mousebutton.h"

String_bimap <sf::Mouse::Button> Mousebutton::buttoncodes{ sf::Mouse::Button::ButtonCount };

const std::string& Mousebutton::to_string(sf::Mouse::Button buttoncode)
{
	if (buttoncodes.size() == 0)
	{
		init();
	}
	return buttoncodes.at(buttoncode);
}

const sf::Mouse::Button Mousebutton::to_buttoncode(const std::string& buttonname)
{
	if (buttoncodes.size() == 0)
	{
		init();
	}
	return buttoncodes.at(buttonname);
}

bool Mousebutton::contains(const std::string& buttonname)
{
	return buttoncodes.contains(buttonname);
}

void Mousebutton::init()
{
	buttoncodes.insert(sf::Mouse::Button::Left, "LeftButton");
	buttoncodes.insert(sf::Mouse::Button::Right, "RightButton");
	buttoncodes.insert(sf::Mouse::Button::Middle, "MiddleButton");
	buttoncodes.insert(sf::Mouse::Button::XButton1, "Extra1");
	buttoncodes.insert(sf::Mouse::Button::XButton2, "Extra2");
}
