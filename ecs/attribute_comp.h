#pragma once
#include <memory>
#include <algorithm>

#include "ecs_base.h"
#include "bonus.h"

struct Attribute_data
{
	using Ptr = std::unique_ptr<Attribute_data>;
	unsigned int base;
	unsigned int units_per_point;
	int bought;
	int received;
	std::vector<std::string> notes;
	std::vector<Bonus> bonuses;
};

enum class Attribute { ST, DX, IQ, HT, HP, Will, Per, FP, ST_Lift, ST_Strike, BL };
Attribute& operator++(Attribute& a);

struct Attribute_comp : public cloneable<Attribute_comp, Component_base>
{
	Attribute_comp (const Attribute_comp& other)
	{
		copy (other);
	}
	Attribute_comp& operator= (const Attribute_comp& other)
	{
		if (!(&other == this))
		{
			copy (other);
		}
		return *this;
	}

	void copy (const Attribute_comp& other);
	int get_natural (Attribute attrib) { return m_attributes[attrib]->base + m_attributes[attrib]->received + m_attributes[attrib]->bought; }
	
	using Ptr = std::shared_ptr<Attribute_comp>;
	static Ptr create () { return std::make_shared<Attribute_comp> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override {}
	Attribute_comp ()
	{
		m_mutable = true;
		for (Attribute a = Attribute::ST; m_attributes.count (a) == 0; ++a)
		{
			m_attributes[a] = std::make_unique<Attribute_data> ();
		}
	}

	std::unordered_map<Attribute, Attribute_data::Ptr> m_attributes;
};
