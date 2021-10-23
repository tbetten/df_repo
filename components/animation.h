#pragma once
#include <vector>
#include <span>
#include <string>
#include <SFML/Graphics/Rect.hpp>

namespace tiled
{
	struct Animation_frame;
}

struct Shared_context;

struct Frame
{
	Frame () : frame_rect { 0,0,0,0 }, duration { 0 } {}
	Frame (sf::IntRect r, int d) : frame_rect { r }, duration { d }{}
	sf::IntRect frame_rect;
	int duration;
};

struct Animation
{
	void reset();
	void load (std::span<Frame> frames);
	void load (const std::string& key, Shared_context* context) {}

	std::vector<Frame> frames {};
	int current_frame { 0 };
	float time_in_frame { 0.0f };
	bool active{ true };
};