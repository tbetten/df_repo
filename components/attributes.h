#pragma once
#include <memory>
#include <unordered_map>

struct Attribute_data
{
	unsigned int base;
	int bought;
	unsigned int units_per_point;
	int max;
};

enum class Attribute { ST, DX, IQ, HT, HP, Will, Per, FP, ST_Lift, ST_Strike, BM, BS };
Attribute& operator++(Attribute& a);

struct Attributes
{
	Attributes();
	void reset();
	std::vector<Attribute_data> data;
};