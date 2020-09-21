#include "attributes.h"
#include "ecs.h"
#include "components.h"
#include "character.h"

#include <numeric>
#include <stdexcept>
#include <cassert>
#include <span>

namespace attributes
{
	static std::map<std::string, Attrib> string_to_attribute = { {"ST", Attrib::ST}, {"DX", Attrib::DX}, {"IQ", Attrib::IQ}, {"HT", Attrib::HT}, {"HP", Attrib::HP}, {"Will", Attrib::Will}, {"Per", Attrib::Per}, {"FP", Attrib::FP}, {"Basic speed", Attrib::BS}, {"Basic move", Attrib::BM}, {"Size Modifier", Attrib::SM}, {"Basic Lift", Attrib::BL}, {"Lifting Strenth", Attrib::ST_lift}, {"Striking Strength", Attrib::ST_strike} };
	static std::map<Attrib, int> base_values{ {Attrib::ST, 1000}, {Attrib::DX, 1000}, {Attrib::IQ, 1000}, {Attrib::HT, 1000}, {Attrib::HP, 1000}, {Attrib::Will, 1000}, {Attrib::Per, 1000}, {Attrib::FP, 1000}, {Attrib::BS, 500}, {Attrib::BM, 500}, {Attrib::SM, 0} };
	static std::map<Attrib, int> units_per_point_map{ {Attrib::ST, 10}, {Attrib::DX, 5}, {Attrib::IQ, 5}, {Attrib::HT, 10}, {Attrib::HP, 50}, {Attrib::Will, 20 }, { Attrib::Per, 20 }, { Attrib::FP, 25}, {Attrib::BS, 5}, {Attrib::BM,20}, {Attrib::SM, 0} };

	Attrib string_to_attrib(const std::string& attribute_name)
	{
		if (string_to_attribute.contains(attribute_name))
		{
			return string_to_attribute[attribute_name];
		}
		return Attrib::Invalid;
	}

	std::string attrib_to_string(const Attrib attrib)
	{
		auto itr = std::find_if(std::cbegin(string_to_attribute), std::cend(string_to_attribute), [attrib](const std::pair<std::string, Attrib>& entry) {return entry.second == attrib; });
		if (itr == std::cend(string_to_attribute)) return "";
		return itr->first;
	}

	std::vector<Transaction> get_transactions(const Transactions& transactions, const Attrib attrib)
	{
		std::vector<Transaction> results{};
		std::copy_if(std::cbegin(transactions), std::cend(transactions), std::back_inserter(results), [attrib](Transaction transaction) {return transaction.attribute == attrib; });
		return results;
	}

	int get_derived_attribute(const std::span<Transaction> transactions, const Attrib attrib)   //(const Transactions& transactions, const Attrib attrib)
	{
		int strength{ 0 };
		int lifting_str{ 0 };
		switch (attrib)
		{
		case Attrib::BL:
			strength = get_total_value(transactions, Attrib::ST);
			lifting_str = get_total_value(transactions, Attrib::ST_lift);
			return (strength + lifting_str) * (strength + lifting_str) / 500;
		case Attrib::Dodge:
			return (get_total_value(transactions, Attrib::BS) + 300);
		default:
			return 0;
		}
	}

	int get_base_value(const std::span<Transaction> transactions, const Attrib attrib)  //(const Transactions& transactions, const Attrib attrib)
	{
		auto base = std::accumulate(std::cbegin(transactions), std::cend(transactions), base_values[attrib], [attrib](int current_total, const Transaction& transaction) { return (transaction.attribute == attrib && transaction.type == Transaction_type::Raise_base) ? current_total + transaction.units : current_total; });
		if (attrib == Attrib::BM)
		{
			base = (base / 100) * 100;
		}
		return base;
	}

	std::tuple<int, int> get_encumbered_value(const std::span<Transaction> transactions, const Character::Encumbrance encumbrance)  //(const Transactions& transactions, Character::Encumbrance encumbrance)
	{
		int dodge = get_total_value(transactions, Attrib::Dodge);
		int move = get_total_value(transactions, Attrib::BM);
		switch (encumbrance)
		{
		case Character::Encumbrance::No:
			break;
		case Character::Encumbrance::Light:
			move = static_cast<int>(move * 0.8);
			dodge -= 100;
			break;
		case Character::Encumbrance::Medium:
			move = static_cast<int>(move * 0.6);
			dodge -= 200;
			break;
		case Character::Encumbrance::Heavy:
			move = static_cast<int>(move * 0.4);
			dodge -= 300;
			break;
		case Character::Encumbrance::Extra_heavy:
			move = static_cast<int>(move * 0.2);
			dodge -= 400;
			break;
		default:
			break;
		}
		return std::make_tuple(move / 100, dodge);
	}

	int get_basic_move(const std::span<Transaction> transactions) //(const Transactions& transactions)
	{
		auto base = get_base_value(transactions, Attrib::BM);
		return std::accumulate(std::cbegin(transactions), std::cend(transactions), base, [](int current_total, const Transaction& transaction) {return (transaction.attribute == Attrib::BM && transaction.type != Transaction_type::Raise_base) ? current_total + transaction.units : current_total; });
	}



	int get_total_value(const std::span<Transaction> transactions, const Attrib attrib)  //(const Transactions& transactions, const Attrib attrib)
	{
		if (attrib == Attrib::BM) return get_basic_move(transactions);
		if (std::find(std::cbegin(derived_attributes), std::cend(derived_attributes), attrib) != std::cend(derived_attributes)) return get_derived_attribute(transactions, attrib);
		return std::accumulate(std::cbegin(transactions), std::cend(transactions), base_values[attrib], [attrib](int current_total, const Transaction& transaction) { return attrib == transaction.attribute ? current_total + transaction.units : current_total; });
	}

	int get_spent_points(const std::span<Transaction> transactions, const Attrib attrib)  //(const Transactions& transactions, const Attrib attrib)
	{
		return std::accumulate(std::cbegin(transactions), std::cend(transactions), 0, [attrib](int current_total, const Transaction& transaction) {return attrib == transaction.attribute ? current_total + transaction.points_spent : current_total; });
	}

	int units_per_point(const Attrib attrib)
	{
		if (units_per_point_map.contains(attrib))
		{
			return units_per_point_map[attrib];
		}
		throw std::out_of_range("bad attribute " + static_cast<int>(attrib));
	}

	int points_per_unit(const Attrib attrib)
	{
		if (units_per_point_map.contains(attrib))
		{
			if (units_per_point_map[attrib] == 0) return 0;
			return 100 / units_per_point_map[attrib];
		}
		throw std::out_of_range("bad attribute " + static_cast<int>(attrib));
	}

	std::vector<std::tuple<Attrib, int>> raises_base(const Attrib attrib)
	{
		std::vector<std::tuple<Attrib, int>> results{};
		switch (attrib)
		{
		case Attrib::ST:
			results.emplace_back(std::make_tuple(Attrib::HP, 100));
			break;
		case Attrib::DX:
			results.emplace_back(std::make_tuple(Attrib::BS, 25));
			results.emplace_back(std::make_tuple(Attrib::BM, 25));
			break;
		case Attrib::IQ:
			results.emplace_back(std::make_tuple(Attrib::Will, 100));
			results.emplace_back(std::make_tuple(Attrib::Per, 100));
			break;
		case Attrib::HT:
			results.emplace_back(std::make_tuple(Attrib::FP, 100));
			results.emplace_back(std::make_tuple(Attrib::BS, 25));
			results.emplace_back(std::make_tuple(Attrib::BM, 25));
			break;
		default:
			break;
		}
		return results;
	}
}