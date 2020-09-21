#include "animation.h"
#include "Tilesheet.h"
#include <algorithm>

void Animation::reset()
{
	current_frame = 0;
	time_in_frame = 0.0f;
}

void Animation::load (std::span<Frame> s_frames)
{
	//std::for_each (std::cbegin (s_frames), cend (s_frames), [this] (const tiled::Animation_frame& f){frames.emplace_back (f.tile_id, f.duration); });
	std::copy (std::cbegin (s_frames), std::cend (s_frames), std::back_inserter (frames));
}