#pragma once
#include <type_traits>

enum class Direction { North, Northeast, Southeast, South, Southwest, Northwest };

using Underlying = typename std::underlying_type_t<Direction>;

Direction& operator++(Direction& d);

Direction operator++ (Direction& d, int);

Direction& operator--(Direction& d);

Direction operator-- (Direction& d, int);
