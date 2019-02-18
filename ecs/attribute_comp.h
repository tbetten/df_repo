#pragma once
#include <memory>
#include <algorithm>

#include "ecs_base.h"
#include "bonus.h"

struct Attribute_data
{
	Attribute_data () : base{ 0 }, units_per_point{ 0 }, bought{ 0 }, received{ 0 } {}
	using Ptr = std::unique_ptr<Attribute_data>;
	unsigned int base;
	unsigned int units_per_point;
	int bought;
	int received;
	std::vector<std::string> notes;
	std::vector<Bonus> bonuses;
};

enum class Attribute { ST, DX, IQ, HT, HP, Will, Per, FP, ST_Lift, ST_Strike, BL, BM, BS };
Attribute& operator++(Attribute& a);

struct Attribute_comp : public Component_base
{
	int get_natural (Entity_id entity, Attribute attrib) { return m_data[entity][attrib]->base + m_data[entity][attrib]->received + m_data[entity][attrib]->bought; }
	
	using Ptr = std::shared_ptr<Attribute_comp>;
	static Ptr create () { return std::make_shared<Attribute_comp> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override {}
	void add_entity (Entity_id entity) override;
	bool has_entity(Entity_id entity) override {return m_data.count(entity); }

	using attribute_table = std::unordered_map<Attribute, Attribute_data::Ptr>;
	std::unordered_map<Entity_id, attribute_table> m_data;

private:
	attribute_table init_attributes ();
};
