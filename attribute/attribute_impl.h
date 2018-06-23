#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>
namespace attrib
{
	class Attribute
	{
	public:
		Attribute(int base, int price, int bought = 0) : m_base{ base }, m_price{ price }, m_bought{ bought } {}
		Attribute() :Attribute(10, 0) {}
		int add_bonus(Bonus::Ptr bonus);
		void remove_bonus(const int id);
		void buy(const int amount, int& budget);
		void pass_time(const int seconds);
		int get_natural_value() const;
		int get_effective_value() const;
		int get_base() const { return m_base; }
		int get_bought() const { return m_bought; }
		int get_price() const { return m_price; }
		std::vector<std::string> get_messages() const;
		virtual void add_parent(std::shared_ptr<Attribute> attrib) { }
		virtual void recalculate_base() {}
	protected:
		int recalculate() const;
		int m_base;
		int m_price;
		int m_bought;
		int m_max_id = 0;
		std::unordered_map<int, std::unique_ptr<Bonus>> m_bonuses;
	};

	class Dependent_attribute : public Attribute
	{
	public:
		//	enum class Depends_on {Natural, Effective};
		//Dependent_attribute(Depends_on type, int price, int bought = 0) : Attribute{ 0, price, bought } { m_type = type; }
		Dependent_attribute(int price, int bought = 0, int multiplier = 1) : Attribute{ 0, price, bought } { m_multiplier = multiplier; }
		void add_parent(std::shared_ptr<Attribute> attrib) override { m_parents.push_back(attrib); }
		void recalculate_base() override;
	private:
		//	Depends_on m_type;
		int m_multiplier;
		std::vector<std::shared_ptr<Attribute>> m_parents;
	};
}