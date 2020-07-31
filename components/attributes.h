#pragma once
#include "ecs_types.h"
#include "character.h"
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <span>

namespace ecs
{
	class Entity_manager;
}

struct Attributes;

namespace attributes
{
	enum class Attrib { Invalid = -1, ST, DX, IQ, HT, HP, Will, Per, FP, ST_lift, ST_strike, BS, BM, BL, Dodge, SM };
	static std::vector<Attrib> primary_attributes{ Attrib::ST, Attrib::DX, Attrib::IQ, Attrib::HT };
	static std::vector<Attrib> secundary_attributes{ Attrib::HP, Attrib::Will, Attrib::Per, Attrib::FP };
	static std::vector<Attrib> tertiary_attributes{ Attrib::BS, Attrib::BM, Attrib::SM };
	static std::vector<Attrib> strength_modifiers{ Attrib::ST_lift, Attrib::ST_strike };
	static std::vector<Attrib> derived_attributes{ Attrib::BL, Attrib::Dodge };
	//enum class Primary_attribute { ST, DX, IQ, HT };
	//enum class Secundaty_attribute { HP, Will, Per, FP };
	//enum class Strength_modifiers { ST_lift, ST_strike };

	enum class Transaction_type { Buy, Raise_base, Equipment, Effect };
	enum class Template_type { Race, Occupation };

	struct Transaction
	{
		Attrib attribute;
		Transaction_type type;
		Template_type template_type;
		std::string template_name;
		int units;
		int points_spent;
		unsigned int transaction_id; // used for removing transactions, is entity id for equipment and effect id for effects, not used for buy or raise base.
	};

	using Transactions = std::vector<Transaction>;

	std::vector<Transaction> get_transactions(const Transactions& transactions, const Attrib attrib);
	Attrib string_to_attrib(const std::string& attrib_name);
	std::string attrib_to_string(const Attrib attrib);
	int get_base_value(const std::span<Transaction> transactions, const Attrib attrib); //(const Transactions& transactions, const Attrib attrib);
	int get_total_value(const std::span<Transaction> transactions, const Attrib attrib);  //(const Transactions& transactions, const Attrib attrib);
	std::tuple<int, int> get_encumbered_value(const std::span<Transaction> transactions, const Character::Encumbrance encumbrance);  //(const Transactions& transactions, const Character::Encumbrance encumbrance);
	int get_spent_points(const std::span<Transaction>transactions, const Attrib attrib);  //(const Transactions& transactions, const Attrib attrib);
	int units_per_point(const Attrib attrib);
	int points_per_unit(const Attrib attrib);
	std::vector<std::tuple<Attrib, int>> raises_base(const Attrib attrib);
}

struct Attributes
{
	void reset() {}
	std::vector<attributes::Transaction> transactions;
};