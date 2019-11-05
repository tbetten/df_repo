#pragma once
#include "state.h"
#include "db.h"
#include "ecs_types.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

namespace db
{
	class db_connection;
}

struct Attrib_value
{
	std::string name;
	int base;
	int modifier;
};

class State_character_generator : public State
{
public:
	explicit State_character_generator(Shared_context* context);// : State{ context } {}

	void on_create() override;
	void on_destroy() override;
	void activate() override;
	void deactivate() override;
	void update(const sf::Time& time) override;
	void handle_sfml_event(sf::Event& e) override;
	void draw() override;

	void to_game();

private:
	sfg::Box::Ptr read_races();
	void read_attributes();
	void read_attrib_modifiers(const std::string& race);
	void apply_attrib_modifier(const std::string& name, int modifier);
	sfg::Table::Ptr create_attribute_table();
	void on_race_toggle(const std::string& name);
	void on_keep();
	void buy_attrib(Attrib_value v, ecs::Entity_id entity);

	unsigned int m_cp;
	std::vector<Attrib_value> m_attrib_values;
	std::unordered_map<std::string, sfg::RadioButton::Ptr> m_race_buttons;
	std::unique_ptr<db::db_connection> m_db;
	sfg::SFGUI m_sfgui;
	sfg::Window::Ptr m_gui_window;
	sfg::Desktop m_desktop;
	sfg::Entry::Ptr m_name_entry;
	sfg::Label::Ptr m_race;
	sfg::RadioButton::Ptr m_male;
	sfg::RadioButton::Ptr m_female;

	sfg::Box::Ptr m_root_widget;
	sfg::Box::Ptr m_attrib_box;

	bool m_done{ false };
};