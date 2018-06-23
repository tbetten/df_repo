#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace attrib
{
	enum class Attribute_type { ST, DX, IQ, HT, ST_lift, ST_strike, HP, Will, Per, FP, BS, BM };

	class Bonus
	{
	public:
		using Ptr = std::unique_ptr<Bonus>;
		static Ptr create(std::string message, int base_value, int multiplier, bool timed = false, int remaining_time = 0) { return std::make_unique<Bonus>(message, base_value, multiplier, timed, remaining_time); }
		Bonus(std::string message, int base_value = 0, int multiplier = 0, bool timed = false, int remaining_time = 0) : m_message{ message }, m_base{ base_value }, m_multiplier{ multiplier }, m_timed{ timed }, m_remaining_time{ remaining_time } {}
		int get_base() const { return m_base; }
		int get_multiplier() const { return m_multiplier; }
		int is_timed() const { return m_timed; }
		std::string get_message() const { return m_message; }
		int pass_time(const int seconds);
		int remaining_time() const { return m_remaining_time; }
	private:
		std::string m_message;
		int m_base;
		int m_multiplier;
		int m_remaining_time;
		bool m_timed;
	};

	struct Attribute_data
	{
		int base;
		int price;
		int bought;
		int effective;
		std::vector<std::string> messages;
	};

	inline std::ostream& operator<< (std::ostream& os, const Attribute_data& data)
	{
		os << "base: " << data.base << "\t price: " << data.price << "\t bought: " << data.bought << "\t effective: " << data.effective << std::endl;
		for (auto msg : data.messages)
		{
			os << msg << std::endl;
		}
		return os;
	}

	class Attribute_set
	{
	public:
		enum class Value_type { Natural, Effective };
		enum class Encumbrance_level { None, Light, Medium, Heavy, Extra_Heavy, Forbidden };
		Attribute_set();
		~Attribute_set();
		void buy(Attribute_type attrib, int amount, int& budget);
		int add_bonus(const Attribute_type attrib, Bonus::Ptr& bonus);
		void remove_bonus(Attribute_type attrib, int id);
		void pass_time(const int seconds);
		int get_value(Attribute_type attrib, Value_type type) const;
		float get_basic_lift() const;
		Encumbrance_level get_encumbrance(const float weight_carried) const;
		int get_move(const float weight_carried) const;
		int get_dodge() const;
		Attribute_data get_attribute_data(Attribute_type attrib) const;
	private:
		class impl;
		std::unique_ptr<impl> p_impl;
	};
}