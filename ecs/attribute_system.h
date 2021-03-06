/*#pragma once
#include "system.h"
#include "attribute_comp.h"

struct Attrib_payload
{
	Entity_id entity;
	Attribute attribute;
	int units;
};

struct Bonus_totals
{
	int direct = 0;
	int percentage = 0;
};

class Attribute_system : public System_base
{
public:
	using Ptr = std::unique_ptr<Attribute_system>;
	enum class Value_type {Base, Bought, Bonus, Natural, Effective};
	explicit Attribute_system (System_manager* mgr) : System_base (System::Attribute, mgr), m_eventnames{ "attribute_bought", "attribute_changed" }
	{
		Bitmask m;
		m.set (to_number (Component::Attributes));
		m_required_components.push_back (m);
		m_component = mgr->get_entity_mgr ()->get_component<Attribute_comp> (Component::Attributes);
	}
	static Ptr create (System_manager* mgr) { return std::make_unique<Attribute_system> (mgr); }

	void setup_events () override;
	void update (int dt) override;
	void reset (Entity_id entity);
	void spend_points (Entity_id entity, Attribute attrib, int amount);
	int get_value (Entity_id entity, Attribute attrib, Value_type value);
	Dispatcher& get_event (std::string event) override;
private:
	void on_attrib_bought (std::any val);
	int calculate_bonus (const Attribute_data& data) const;
	
	const std::vector<std::string> m_eventnames;
	std::unordered_map<std::string, Dispatcher> m_dispatchers;
	Attribute_comp::Ptr m_component;
};*/