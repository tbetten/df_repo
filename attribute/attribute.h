#pragma once
#include <unordered_map>
#include <memory>

namespace Attributes
{
	class Attribute_base
	{
	public:
		Attribute_base(int value, int multiplier = 0) : m_base_value{ value }, m_base_multiplier{ multiplier } {}
		int get_base_value() const { return m_base_value; }
		int get_base_multiplier() const { return m_base_multiplier; }
	protected:
		int m_base_value;
		int m_base_multiplier;
	};

	class Raw_bonus : public Attribute_base
	{
	public:
		Raw_bonus(int value = 0, int multiplier = 0) : Attribute_base{ value, multiplier } {}
	};

	class Final_bonus : public Attribute_base
	{
	public:
		Final_bonus(int time, int value = 0, int multiplier = 0) : Attribute_base{ value, multiplier } 
		{
			m_remaining_secs = time;
		}
		int pass_time(int seconds) { return m_remaining_secs -= seconds; }
		int get_remaining() const { return m_remaining_secs; }
	private:
		int m_remaining_secs;
	};

	using Bonus_map = std::unordered_map<int, Attribute_base>;
	using Bonus_pair = std::pair<int, Attribute_base>;

	class Attribute : public Attribute_base, public std::enable_shared_from_this<Attribute>
	{
	public:
		using Ptr = std::shared_ptr<const Attribute>;
		Attribute(int base) : Attribute_base{ base } {}
		Attribute() : Attribute_base{ 0 } {}
		Ptr get_ptr() const { return shared_from_this(); }
		void buy(int amount) { m_bought += amount; }
		int add_raw_bonus(Raw_bonus bonus);
		int add_final_bonus(Final_bonus bonus);
		void remove_raw_bonus(int id) { m_raw_bonuses.erase(id); }
		void remove_final_bonus(int id) { m_final_bonuses.erase(id); }
		int get_natural_value() const { return m_base_value + m_bought; }
		int get_final_value()  { return calculate_value(); }
	protected:
		void apply_raw_bonuses();
		void apply_final_bonuses();
		virtual int calculate_value() ;
		int m_final;
	private:
		

		Bonus_map m_raw_bonuses;
		Bonus_map m_final_bonuses;
		int m_bought{ 0 };
		int m_max_id{ 0 };
	};

	class Dependent_attribute : public Attribute
	{
	public:
		Dependent_attribute(int base) : Attribute{ base } {}
		void add_attribute(Attribute::Ptr attr) { m_parents.push_back(attr); }
		void remove_attribute(int id);
	protected:
		std::vector<Attribute::Ptr> m_parents;
	};

	class Depends_on_natural : public Dependent_attribute
	{
	public:
		Depends_on_natural(int base) : Dependent_attribute{ base } {}
		int calculate_value() override;

	};

	class Attribute_set
	{
	public:
		enum class Attrib {ST, ST_Lift, ST_Strike, DX, IQ, HT};
		Attribute_set();
		int get_natural_value(Attrib attrib) { return m_attribs[attrib]->get_natural_value(); }
		void buy(Attrib attrib, int amount, int budget) { return m_attribs[attrib]->buy(amount); }
	private:
		std::unordered_map<Attrib, Attribute::Ptr> m_attribs;
	};
}