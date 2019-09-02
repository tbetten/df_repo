#include "attributes.h"

Attributes::Attributes()
{
	data.reserve(12);
	reset();
}
void Attributes::reset()
{
	for (Attribute a = Attribute::ST; a != Attribute::BS; ++a)
	{
		int index = static_cast<int>(a);
		data[index].bought = 0;
		data[index].base = 1000;
		switch (a)
		{
		case Attribute::ST:
			[[fallthrough]] ;
		case Attribute::HT:
			data[index].units_per_point = 10;
			break;
		case Attribute::DX:
			[[fallthrough]] ;
		case Attribute::IQ:
			data[index].units_per_point = 5;
			break;
		case Attribute::HP:
			data[index].units_per_point = 50;
			break;
		case Attribute::Will:
			[[fallthrough]] ;
		case Attribute::Per:
			data[index].units_per_point = 20;
			break;
		case Attribute::FP:
			data[index].units_per_point = 33;
			break;
		case Attribute::BS:
			data[index].base = (data[static_cast<int>(Attribute::DX)].base + data[static_cast<int>(Attribute::HT)].base) / 4;
			data[index].units_per_point = 5;
			break;
		case Attribute::BM:
			data[index].base = ((data[static_cast<int>(Attribute::DX)].base + data[static_cast<int>(Attribute::HT)].base) / 400) * 100;
			data[index].units_per_point = 20;
			break;
		default:
			break;
		}
	}
}

Attribute& operator++(Attribute& a)
{
	return a = (a == Attribute::BS) ? Attribute::ST : static_cast<Attribute>(static_cast<int>(a) + 1);
}
