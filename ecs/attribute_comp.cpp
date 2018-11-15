#include "stdafx.h"
#include "attribute_comp.h"

void Attribute_comp::copy (const Attribute_comp& other)
{
	for (auto& attrib : other.m_attributes)
	{
		m_attributes[attrib.first] = std::make_unique<Attribute_data> (*attrib.second);
	}
	m_mutable = other.m_mutable;
}

Attribute& operator++(Attribute& a)
{
	return a = (a == Attribute::BL) ? Attribute::ST : static_cast<Attribute>(static_cast<int>(a) + 1);
}