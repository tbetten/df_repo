#pragma once
#include <array>


extern constexpr double dice_mean (int dice);
extern constexpr double dice_stddev (int dice);
namespace damage
{

	struct Dice
	{
		int dice;
		int modifier;
	};

	struct Dice_entry
	{
		constexpr Dice_entry () : dice { 1 }, mean { dice_mean (1) }, stddev { dice_stddev (1) }{}
		constexpr Dice_entry (int num) : dice { num }, mean { dice_mean (num) }, stddev { dice_stddev (num) }{}

		int dice;
		double mean;
		double stddev;
	};

	struct Dice_table
	{
		constexpr Dice_table ()
		{
			for (int i = 0; i < 20; ++i)
			{
				table [i] = Dice_entry { i };
			}
		}
		std::array<Dice_entry, 20> table;
	};

	constexpr std::array<Dice, 41> thrust_table
	{
		Dice{0,0},
		Dice{1,-6}, Dice{1,-6},
		Dice{1,-5}, Dice{1,-5},
		Dice{1,-4}, Dice{1,-4},
		Dice{1,-3}, Dice{1,-3},
		Dice{1,-2}, Dice{1,-2},
		Dice{1,-1}, Dice{1,-1},
		Dice{1,0}, Dice{1,0},
		Dice{1,1},Dice{1,1},
		Dice{1,2},Dice{1,2},
		Dice{2,-1}, Dice{2,-1},
		Dice{2,0}, Dice{2,0},
		Dice{2,1},Dice{2,1},
		Dice{2,2}, Dice{2,2},
		Dice{3,-1}, Dice{3,-1},
		Dice{3,0}, Dice{3,0},
		Dice{3,1}, Dice{3,1},
		Dice{3,2},Dice{3,2},
		Dice{4,-1}, Dice{4,-1},
		Dice{4,0},Dice{4,0},
		Dice{4,1},Dice{4,1}
	};

	constexpr std::array<Dice, 41> swing_table
	{
		Dice{0,0},
		Dice{1,-5}, Dice{1,-5},
		Dice{1,-4}, Dice{1,-4},
		Dice{1,-3},Dice{1,-3},
		Dice{1,-2},Dice{1,-2},
		Dice{1,-1}, Dice{1,0},
		Dice{1,1}, Dice{1,2},
		Dice{2,-1}, Dice{2,0},
		Dice{2,1}, Dice{2,2},
		Dice{3,-1}, Dice{3,0},
		Dice{3,1}, Dice{3,2},
		Dice{4,-1}, Dice{4,0},
		Dice{4,1}, Dice{4,2},
		Dice{5,-1}, Dice{5,0},
		Dice{5,1}, Dice{5,1},
		Dice{5,1}, Dice{5,2},
		Dice{6,-1}, Dice{6,-1},
		Dice{6,0},Dice{6,0},
		Dice{6,1},Dice{6,1},
		Dice{6,2},Dice{6,2},
		Dice{7,-1},Dice{7,-1}
	};

	constexpr double get_mean (Dice d)
	{
		return dice_mean (d.dice) + d.modifier;
	}

	struct Damage_entry
	{
		constexpr Damage_entry () : strength { 0 }, thrust { Dice{0,0} }, swing { Dice{ 0,0 } }, thrust_mean { 0.0 }, thrust_stddev { 0.0 }, swing_mean { 0.0 }, swing_stddev { 0.0 }{}
		constexpr Damage_entry (int st) : strength { st }, thrust { thrust_table.at (st) }, swing { swing_table.at (st) }, thrust_mean { get_mean (thrust_table.at (st)) }, thrust_stddev { dice_stddev (thrust_table.at (st).dice) }, swing_mean { get_mean (swing_table.at (st)) }, swing_stddev { dice_stddev (swing_table.at (st).dice) }  {}

		int strength;
		Dice thrust;
		Dice swing;
		double thrust_mean;
		double thrust_stddev;
		double swing_mean;
		double swing_stddev;
	};

	struct Damage_table
	{
		constexpr Damage_table ()
		{
			for (int i = 0; i < 42; ++i)
			{
				table [i] = Damage_entry { i };
			}
		}
		std::array<Damage_entry, 41> table;
	};
}