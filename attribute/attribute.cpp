#include "stdafx.h"
#include "attribute.h"
#include <algorithm>
#include <unordered_map>

namespace attrib
{
	int Attribute::add_raw_bonus(Raw_bonus bonus)
	{
		m_raw_bonuses.emplace(++m_max_id, bonus);
		return m_max_id;
	}

	int Attribute::add_final_bonus(Final_bonus bonus)
	{
		m_final_bonuses.emplace(++m_max_id, bonus);
		return m_max_id;
	}

	std::pair<int, int> add_bonuses(Bonus_map map)
	{
		int values{ 0 };
		int multis{ 0 };
		std::for_each(map.cbegin(), map.cend(), [&values, &multis](Bonus_pair elem)
		{
			values += elem.second.get_base_value();
			multis += elem.second.get_base_multiplier();
		});
		return std::make_pair(values, multis);
	}

	void Attribute::apply_raw_bonuses()
	{
		auto ret = add_bonuses(m_raw_bonuses);
		m_final += ret.first;
		m_final *= (1 + (ret.second / 100));
	}

	void Attribute::apply_final_bonuses()
	{
		auto ret = add_bonuses(m_final_bonuses);
		m_final += ret.first;
		m_final *= (1 + (ret.second / 100));
	}

	int Attribute::calculate_value() 
	{
		int m_final = m_base_value + m_bought;
		apply_raw_bonuses();
		apply_final_bonuses();
		return m_final;
	}

	int Depends_on_natural::calculate_value()
	{
		for (auto parent : m_parents)
		{
			m_base_value += parent->get_natural_value();
		}
		return m_base_value;
	}

	Attribute_set::Attribute_set()
	{
		m_attribs[Attrib::ST] = std::make_shared<Attribute> (10);
		m_attribs[Attrib::DX] = std::make_shared<Attribute> (20);
		m_attribs[Attrib::IQ] = std::make_shared<Attribute> (20);
		auto q = Depends_on_natural{ 0 };
		q.add_attribute(m_attribs[Attrib::ST]->get_ptr());
	}
}