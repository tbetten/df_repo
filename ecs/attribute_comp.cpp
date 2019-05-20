/*#include "stdafx.h"
#include "attribute_comp.h"

void Attribute_comp::add_entity (Entity_id entity)
{
	if (m_data.count (entity)) return;
	m_data[entity] = init_attributes ();
}

Attribute_comp::attribute_table Attribute_comp::init_attributes ()
{
	attribute_table table;
	for (Attribute a = Attribute::ST; table.count (a) == 0; ++a)
	{
		table[a] = std::make_unique<Attribute_data> ();
		switch (a)
		{
		case Attribute::ST: [[fallthrough]];
		case Attribute::HT:
			table[a]->base = 1000;
			table[a]->units_per_point = 10;
			break;
		case Attribute::DX: [[fallthrough]];
		case Attribute::IQ:
			table[a]->base = 1000;
			table[a]->units_per_point = 5;
			break;
		case Attribute::HP:
			table[a]->base = table[Attribute::ST]->base;
			table[a]->units_per_point = 50;
			break;
		case Attribute::Will:  [[fallthrough]];
		case Attribute::Per:
			table[a]->base = table[Attribute::IQ]->base;
			table[a]->units_per_point = 20;
			break;
		case Attribute::FP:
			table[a]->base = table[Attribute::HT]->base;
			table[a]->units_per_point = 33;
			break;
		case Attribute::ST_Lift:  [[fallthrough]];
		case Attribute::ST_Strike:
			table[a]->base = table[Attribute::ST]->base;
			break;
		case Attribute::BL:
			break;
		case Attribute::BS:
			table[a]->base = (table[Attribute::DX]->base + table[Attribute::HT]->base) / 4;
			table[a]->units_per_point = 5;
		default:
		case Attribute::BM:
			table[a]->base = ((table[Attribute::DX]->base + table[Attribute::HT]->base) / 400) * 100;
			table[a]->units_per_point = 20;
			break;
		}

	}
	return table;
}

Attribute& operator++(Attribute& a)
{
	return a = (a == Attribute::BS) ? Attribute::ST : static_cast<Attribute>(static_cast<int>(a) + 1);
}*/