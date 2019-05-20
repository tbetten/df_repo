#include "stdafx.h"
#include "attribute.h"
#include "attribute_impl.h"
#include <stdexcept>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <iostream>

/*
Nieuwe gedachte: dependents werken alleen op natural value, dus base + bought van de primary
is de base van de secondary.
Een bonus op een primary geeft een even grote bonus op een secondary in de volgende gevallen:
- ST -> ST_lift en ST_strike
- IQ -> Will
- IQ -> Per
Andere bonussen op primaries geven geen bonus op secondaries. Dit komt overeen met B-421.
Als de bonus op de primary verdwijnt, wordt ook de bonus op de secondary verwijderd.
*/

namespace attrib
{
	using Child_bonuses = std::unordered_map<Attribute_type, std::vector<int>>;
	//using Bonuses_per_incoming = std::unordered_map<int, Child_bonuses>;
	using Bonus_info = std::unordered_map<Attribute_type, std::unordered_map<int, Child_bonuses>>;

	class Attribute_set::impl
	{
	public:
		impl();
		int add_bonus(const Attribute_type attrib, Bonus::Ptr& bonus);
		void remove_bonus(Attribute_type attrib, int id);
		void buy(const Attribute_type attrib, int amount, int& budget);
		void pass_time(const int seconds);
		int get_value(Attribute_type attrib, Value_type type);
		Attribute_data get_attribute_data(Attribute_type attrib);
	private:
		void recalculate();
		int copy_bonus(Attribute_type attrib_to, const Bonus::Ptr& bonus);
		std::unordered_map<Attribute_type, std::vector<Attribute_type>> m_relays;
		std::unordered_map<Attribute_type, std::shared_ptr<Attribute>> m_attributes;
		std::unordered_map<Attribute_type, std::unordered_map<int, Child_bonuses>> m_bonus_ids;
	};

	Attribute_set::Attribute_set() noexcept : p_impl{ new impl{} } {}
	Attribute_set::~Attribute_set() = default;

	Attribute_set::impl::impl()
	{
		m_attributes[Attribute_type::ST] = std::make_shared<Attribute>(10, 10);
		m_attributes[Attribute_type::DX] = std::make_shared<Attribute>(10, 20);
		m_attributes[Attribute_type::IQ] = std::make_shared<Attribute>(10, 20);
		m_attributes[Attribute_type::HT] = std::make_shared<Attribute>(10, 10);

		m_attributes[Attribute_type::ST_lift] = std::make_shared<Dependent_attribute>(2);
		m_attributes[Attribute_type::ST_lift]->add_parent(m_attributes[Attribute_type::ST]);
		m_attributes[Attribute_type::ST_strike] = std::make_shared < Dependent_attribute>(2);
		m_attributes[Attribute_type::ST_strike]->add_parent(m_attributes[Attribute_type::ST]);
		m_attributes[Attribute_type::HP] = std::make_shared < Dependent_attribute>(2);
		m_attributes[Attribute_type::HP]->add_parent(m_attributes[Attribute_type::ST]);
		m_attributes[Attribute_type::Will] = std::make_shared < Dependent_attribute>(5);
		m_attributes[Attribute_type::Will]->add_parent(m_attributes[Attribute_type::IQ]);
		m_attributes[Attribute_type::Per] = std::make_shared < Dependent_attribute>(5);
		m_attributes[Attribute_type::Per]->add_parent(m_attributes[Attribute_type::IQ]);
		m_attributes[Attribute_type::FP] = std::make_shared < Dependent_attribute>(3);
		m_attributes[Attribute_type::FP]->add_parent(m_attributes[Attribute_type::HT]);
		m_attributes[Attribute_type::BS] = std::make_shared < Dependent_attribute>(5);
		m_attributes[Attribute_type::BS]->add_parent(m_attributes[Attribute_type::DX]);
		m_attributes[Attribute_type::BS]->add_parent(m_attributes[Attribute_type::HT]);
		m_attributes[Attribute_type::BM] = std::make_shared < Dependent_attribute>(5, 0, 4);
		m_attributes[Attribute_type::BM]->add_parent(m_attributes[Attribute_type::BS]);
		recalculate();

		m_relays[Attribute_type::ST].push_back(Attribute_type::ST_lift);
		m_relays[Attribute_type::ST].push_back(Attribute_type::ST_strike);
		m_relays[Attribute_type::IQ].push_back(Attribute_type::Will);
		m_relays[Attribute_type::IQ].push_back(Attribute_type::Per);
	}

	int Attribute_set::add_bonus(const Attribute_type attrib, Bonus::Ptr& bonus)
	{
		return p_impl->add_bonus(attrib, bonus);
	}

	void Attribute_set::remove_bonus(Attribute_type attrib, int id)
	{
		return p_impl->remove_bonus(attrib, id);
	}

	void Attribute_set::pass_time(const int seconds)
	{
		p_impl->pass_time(seconds);
	}

	void Attribute_set::buy(const Attribute_type attrib, int amount, int& budget)
	{
		return p_impl->buy(attrib, amount, budget);
	}

	int Attribute_set::get_value(Attribute_type attrib, Attribute_set::Value_type value) const
	{
		return p_impl->get_value(attrib, value);
	}

	Attribute_data Attribute_set::get_attribute_data(Attribute_type attrib) const
	{
		return p_impl->get_attribute_data(attrib);
	}

	float Attribute_set::get_basic_lift() const
	{
		auto st = p_impl->get_value(Attribute_type::ST_lift, Value_type::Effective);
		auto bl = (st * st) / 5.0f;
		if (bl > 10.0f)
		{
			return std::roundf(bl);
		}
		else
		{
			return bl;
		}
	}

	Attribute_set::Encumbrance_level Attribute_set::get_encumbrance(const float weight_carried) const
	{
		auto bl = get_basic_lift();
		auto factor = static_cast<int> (std::floor(weight_carried / bl));
		switch (factor)
		{
		case 0:
			return Encumbrance_level::None;
		case 1:
			return Encumbrance_level::Light;
		case 2:
			return Encumbrance_level::Medium;
		case 3:
		case 4:
		case 5:
			return Encumbrance_level::Heavy;
		case 6:
		case 7:
		case 8:
		case 9:
			return Encumbrance_level::Extra_Heavy;
		default:
			return Encumbrance_level::Forbidden;
		}
	}

	int Attribute_set::get_move(const float weight_carried) const
	{
		auto encumbrance = get_encumbrance(weight_carried);
		auto bm = get_value(Attribute_type::BM, Value_type::Effective);
		int move{ 0 };
		switch (encumbrance)
		{
		case Encumbrance_level::None:
			move = bm;
			break;
		case Encumbrance_level::Light:
			move = static_cast<int> (bm * 0.8f);
			break;
		case Encumbrance_level::Medium:
			move = static_cast<int> (bm * 0.6f);
			break;
		case Encumbrance_level::Heavy:
			move = static_cast<int> (bm * 0.4f);
			break;
		case Encumbrance_level::Extra_Heavy:
			move = static_cast<int> (bm * 0.2f);
		}
		return move == 0 ? 1 : move;
	}

	int Attribute_set::get_dodge() const
	{
		auto bs = get_value(Attribute_type::BS, Value_type::Effective);
		auto basic = 3 + (bs / 4);
		return basic;
	}

	int Attribute_set::impl::copy_bonus(Attribute_type attrib_to, const Bonus::Ptr& bonus)
	{
		auto copy = Bonus::create(bonus->get_message(), bonus->get_base(), bonus->get_multiplier(), bonus->is_timed(), bonus->remaining_time());
		return m_attributes[attrib_to]->add_bonus(std::move(copy));
	}

	int Attribute_set::impl::add_bonus(const Attribute_type attrib, Bonus::Ptr& bonus)
	{
		std::unordered_map<Attribute_type, int> child_ids;
		for (auto child_pair : m_relays)
		{
			if (attrib == child_pair.first)  // bonus must be relayed to children
			{
				for (auto child : child_pair.second)
				{
					child_ids[child] = copy_bonus(child, bonus);
				}
			}
		}
		int id = m_attributes[attrib]->add_bonus(std::move(bonus));
		for (auto pair : child_ids)
		{
			m_bonus_ids[attrib][id][pair.first].push_back(pair.second);
		}
		recalculate();
		return id;
	}

	void Attribute_set::impl::remove_bonus(Attribute_type attrib, int id)
	{
		m_attributes[attrib]->remove_bonus(id);
		auto child_bonuses = m_bonus_ids[attrib][id];
		if (!child_bonuses.empty())
		{
			for (auto pair : child_bonuses)
			{
				for (auto c_id : pair.second)
				{
					m_attributes[pair.first]->remove_bonus(c_id);
				}
			}
		}
		recalculate();
	}

	void Attribute_set::impl::pass_time(const int seconds)
	{
		std::for_each(m_attributes.begin(), m_attributes.end(), [seconds](auto& attrib) {attrib.second->pass_time(seconds); });
		recalculate();
	}

	void Attribute_set::impl::buy(const Attribute_type attrib, int amount, int& budget)
	{
		m_attributes[attrib]->buy(amount, budget);
		recalculate();
	}

	int Attribute_set::impl::get_value(const Attribute_type attrib, Value_type value)
	{
		switch (value)
		{
		case Value_type::Natural:
			return m_attributes[attrib]->get_natural_value();
		case Value_type::Effective:
			return m_attributes[attrib]->get_effective_value();
		default:
			return 0;
		}
	}

	Attribute_data Attribute_set::impl::get_attribute_data(const Attribute_type attrib)
	{
		Attribute_data ret;
		ret.base = m_attributes[attrib]->get_base();
		ret.bought = m_attributes[attrib]->get_bought();
		ret.price = m_attributes[attrib]->get_price();
		ret.effective = m_attributes[attrib]->get_effective_value();
		ret.messages = m_attributes[attrib]->get_messages();
		return ret;
	}

	void Attribute_set::impl::recalculate()
	{
		for (auto& attrib : m_attributes)
		{
			attrib.second->recalculate_base();
		}
	}

	int Bonus::pass_time(const int seconds)
	{
		if (m_timed)
		{
			m_remaining_time -= seconds;
			return m_remaining_time;
		}
		else
		{
			return 1;
		}
	}

	int Attribute::add_bonus(Bonus::Ptr bonus)
	{
		m_bonuses[++m_max_id] = std::move(bonus);
		return m_max_id;
	}

	void Attribute::remove_bonus(const int id)
	{
		m_bonuses.erase(id);
	}

	void Attribute::buy(const int amount, int& budget)
	{
		if (amount * m_price > budget)
		{
			throw std::out_of_range("Not enough budget to buy " + std::to_string(amount) + " points");
		}
		else
		{
			m_bought += amount;
			budget -= (amount * m_price);
		}
	}

	int Attribute::get_natural_value() const
	{
		return m_base + m_bought;
	}

	int Attribute::get_effective_value() const
	{
		return recalculate();
	}

	int Attribute::recalculate() const
	{
		int val{ 0 };
		int mult{ 0 };
		int final{ m_base + m_bought };
		for (auto itr = m_bonuses.begin(); itr != m_bonuses.end(); ++itr)
		{
			val += itr->second->get_base();
			mult += itr->second->get_multiplier();
		}
		final += val;
		final *= static_cast<int>((1 + (mult / 100.0f)));
		return final;
	}


	void Attribute::pass_time(const int seconds)
	{
		for (auto itr = m_bonuses.begin(); itr != m_bonuses.end(); )
		{
			if (itr->second->pass_time(seconds) <= 0)
			{
				itr = m_bonuses.erase(itr);
			}
			else
			{
				++itr;
			}
		}
	}

	std::vector<std::string> Attribute::get_messages() const
	{
		std::vector<std::string> ret;
		for (auto itr = m_bonuses.begin(); itr != m_bonuses.end(); ++itr)
		{
			ret.emplace_back(itr->second->get_message());
		}
		return ret;
	}

	void Dependent_attribute::recalculate_base()
	{
		m_base = 0;
		for (auto parent : m_parents)
		{
			m_base += parent->get_natural_value();
		}
		m_base /= m_multiplier;
	}
}