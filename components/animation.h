#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>

struct Frame
{
	sf::IntRect frame_rect;
	int duration;
};

struct Animation
{
	void reset();
	std::vector<Frame> frames;
	int current_frame;
	float time_in_frame;
};