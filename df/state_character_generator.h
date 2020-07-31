#pragma once
#include "state.h"
#include "db.h"
#include "ecs_types.h"
#include "attributes.h" 

#include <memory>
#include <vector>
#include <tuple>
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

enum class Race : int;
enum class Gender : int;

struct Character_data
{
	Gender gender;
	std::string name;
	Race race;
	std::string tile_sheet;
	int tile_index;
	int beard_index{ -1 };
	std::vector<attributes::Transaction> transactions;
	std::vector<Attrib_value> attrib_modifiers;
};

struct Modifier
{
	Race race;
	attributes::Attrib attribute;
	int amount;
};

struct Chooser
{
	inline void reset()
	{
		value = 0;
		cost_per_step = 0;
	}
	int value;
	std::string id;
	int cost_per_step;
	sfg::SpinButton::Ptr widget;
};

struct Racial_talent
{
	std::string name;
	std::string description;
	Race race;
	int max_levels;
	int default_level;
	int cost_per_level;
	std::vector<std::string> skills;
};

struct Advantage
{
	std::string name;
	std::string description;
	std::variant<Race, Occupation> template_name;
	int default_level;
	int max_level; 
	int cost_per_level;
	bool uses_self_control;
	std::optional<int> self_control;
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
	enum class CP_fractions { Racial_talent, Racial_advantage };
	sfg::Box::Ptr read_races();
	void read_attributes();
	void read_attrib_modifiers();
	void apply_attrib_modifier(const std::string& name, int modifier);
	sfg::Table::Ptr create_attribute_table();
	void create_racial_talent(const std::string& race);
	void on_race_toggle(const std::string& name);
	void on_keep();
	void on_finish();
	void remove_party_members();
	void buy_attrib(Attrib_value v, ecs::Entity_id entity);
	std::string make_cost_label(int level);
	void update_cp_label();
	int find_chooser(const std::string& id);
	void on_chooser_changed(const std::string& id, CP_fractions fraction);
	template <typename T = sfg::Label>
	decltype(auto) find_widget(std::string id)
	{
		return std::dynamic_pointer_cast<T>(sfg::Widget::GetWidgetById(std::move(id)));
	}
	
	unsigned int m_cp;
	std::vector<int> m_cp_fractions;
	int m_cost_per_level{ 0 };
	std::vector<Attrib_value> m_attrib_values;
	std::unordered_map<std::string, sfg::RadioButton::Ptr> m_race_buttons;
	//std::unique_ptr<db::db_connection> m_db;
	db::DB_connection m_db;
	sfg::SFGUI m_sfgui;
	sfg::Window::Ptr m_gui_window;
	sfg::Desktop m_desktop;
	sfg::Entry::Ptr m_name_entry;
	sfg::Label::Ptr m_race;
	sfg::RadioButton::Ptr m_male;
	sfg::RadioButton::Ptr m_female;
	sfg::Label::Ptr m_cp_label;
	std::vector<Chooser> m_choosers;

	sfg::Box::Ptr m_root_widget;
	sfg::Box::Ptr m_attrib_box;

	std::vector<attributes::Transaction> m_attribute_transactions;
	using talent_data = std::tuple<std::string, std::string, std::string, int, int, int>;
	std::vector<Racial_talent> m_racial_talents;
	std::vector<Advantage> m_racial_advantages;

	std::vector<Modifier> m_modifiers;
	std::vector<Character_data> m_party;

	bool m_done{ false };
};