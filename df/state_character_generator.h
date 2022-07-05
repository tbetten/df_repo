#pragma once
#include "state.h"
#include "db.h"
#include "ecs_types.h"
#include "attributes.h" 
#include "skill_comp.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

namespace db
{
	class db_connection;
}

template <typename T = sfg::Label>
decltype(auto) find_widget(std::string id)
{
	return std::dynamic_pointer_cast<T>(sfg::Widget::GetWidgetById(std::move(id)));
}

// model

enum class Template_type { Invalid = -2, None, Race, Occupation };
enum class Gender : int;

struct Attrib_value
{
	std::string name;
	int base;
	int modifier;
};

struct Attrib_modifier
{
	attributes::Attrib m_attribute;
	int m_min_amount;
	int m_max_amount;
	int m_amount;
};

struct Advantage
{
	std::string m_name;
	std::string m_description;
	int m_default_level;
	int m_max_level{ 0 };
	int m_level{ 0 };
	int m_cost_per_level{ 0 };
	int m_cost{ 0 };
	std::optional<int> m_self_control;
	bool m_mandatory;
};

struct Selfcontrol
{
	int key;
	std::string m_name;
	int perc;
	double multiplier;
};

struct Talent
{
	std::string m_name;
	std::string m_description;
	bool upgradeable;
	unsigned int m_value;
	unsigned int m_min_value;
	unsigned int m_max_value;
	unsigned int m_cost_per_level;
	std::vector<std::string> m_skills;
};

struct Skill
{
	std::string m_name;
	Template_type m_template_type;
	skill::Difficulty m_difficulty;
	unsigned int m_value;
	attributes::Attrib m_attribute;
};

struct Template
{
	Template(db::DB_connection::Ptr db, const std::string& name);

	std::string m_name;
	bool m_default;
	Template_type m_type;
	unsigned int m_cost;

	std::vector<Attrib_modifier> m_modifiers;
	std::vector<attributes::Transaction> m_attribute_transactions;
	std::vector<Talent> m_talents;
	std::vector<Skill> m_skills;
	std::vector<Advantage> m_advantages;
	std::vector<Advantage> m_disadvantages;
	std::vector<Selfcontrol> m_selfcontrol_levels;
	unsigned int m_max_disad_cp;
private:
	void read_attrib_modifiers(db::DB_connection::Ptr db);
	void read_talents(db::DB_connection::Ptr db);
	void read_advantages(db::DB_connection::Ptr db);
	void read_skills();
};

class Character_data
{
public:
	std::string m_name;
	Gender m_gender { Gender::Male };
	std::string m_race;
	std::string m_occupation;
	unsigned int m_cp;
	std::vector<attributes::Transaction> m_attribute_transactions;
	std::vector<skill::Transaction> m_skill_transactions;
	std::vector<Talent> m_talents;
	std::vector<Advantage> m_disadvantages;
	std::vector<Advantage> m_advantages;

private:
};

enum class Phase { Name, Race, Occupation };
// view
class Controller;

class Form
{
public:
	using Ptr = std::shared_ptr<Form>;

	virtual ~Form() = default;
	virtual void commit(Character_data& character) = 0;
	virtual sfg::Widget::Ptr widget() = 0;
};

class Name_form : public Form
{
public:
	Name_form();
	static Form::Ptr create();
	sfg::Widget::Ptr widget() override;
	void commit(Character_data& character) override;
private:
	
	sfg::Widget::Ptr m_widget;
};

class Attribute_view
{
public:
	using Ptr = std::shared_ptr<Attribute_view>;
	Attribute_view(std::function<void(const std::string&)>, std::string template_name);
	static Ptr create(std::function<void(const std::string&)> refresh, std::string template_name);
	sfg::Widget::Ptr widget();
	void clear();
	void show(const std::span<attributes::Transaction> transactions);
	void show(std::span<Attrib_modifier> modifiers);
	void commit(Character_data& character);
private:
	void attach_modified_attribute(sf::Uint32 column, sf::Uint32 row, attributes::Attrib attribute, Attrib_modifier* modifier);
	void on_scale_changed(sfg::Scale* scale, sfg::Label* amount, sfg::Label* cost, int ppu, Attrib_modifier* modifier_ptr);
	//void create_attribute_row(unsigned int column, unsigned int row, attributes::Attrib attrib, sfg::Table::Ptr table);
	sfg::Table::Ptr m_widget;
	std::map<attributes::Attrib, sfg::Label::Ptr> m_value_labels;
	std::function<void(const std::string&)> m_refresh_attributes;
	std::string m_template_name;
};

class Talent_view
{
public:
	using Ptr = std::shared_ptr<Talent_view>;
	Talent_view(std::function<void(const std::string&)> refresh_callback, const std::string& template_name);
	static Ptr create(std::function<void(const std::string&)> refresh_callback, const std::string& template_name);
	sfg::Widget::Ptr widget();
	void clear();
	void show(std::span<Talent> talents);
private:
	void on_scale_changed(sfg::Scale* scale, sfg::Label* value, sfg::Label* cost, Talent* talent);
	std::function<void(const std::string&)> m_refresh_callback;
	sfg::Table::Ptr m_widget;
};

class Advantage_view
{
public:
	using Ptr = std::shared_ptr<Advantage_view>;
	Advantage_view(std::function<void(const std::string&)> refresh_callback, const std::string& template_name);
	static Ptr create(std::function<void(const std::string&)> refresh_callback, const std::string& template_name);
	sfg::Widget::Ptr widget();
	void clear();
	void show(std::span<Advantage> advantages, const Template& current_template);
private:
	int count_adv_cp(std::span<Advantage> advantages);
	void on_toggle(std::span<Advantage>advantages, size_t index, unsigned int max_disads);
	void on_selfcontrol_change(const std::string& name, const std::vector<Selfcontrol>& levels, std::span<Advantage> advantage);
	std::function<void(const std::string&)> m_refresh_callback;
	sfg::Table::Ptr m_widget;
	using advantage_pair = std::pair<std::weak_ptr<sfg::CheckButton>, size_t>;
	std::vector<size_t> m_mandatory_advantages;
	std::vector<advantage_pair> m_optional_advantages;
};

class Main_view;

class Template_form : public Form
{
public:
	Template_form(Phase phase, db::DB_connection::Ptr db, std::function<void(const std::string&)> refresh_template_callback, Character_data* character);
	static Form::Ptr create(Phase phase, db::DB_connection::Ptr db, std::function<void(const std::string& refresh_template_callback)>, Character_data* character);
	sfg::Widget::Ptr widget() override;
	void commit(Character_data& character) override;
private:
	void on_template_toggle(const size_t index);
	sfg::Widget::Ptr create_template_table();
	sfg::RadioButton::Ptr m_previous_rb;
	sfg::RadioButton::Ptr m_current_rb;

	size_t m_current_template_index;
	sfg::Widget::Ptr m_widget;
	std::string m_current_template;
	Attribute_view::Ptr m_attribute_table;
	Talent_view::Ptr m_talent_table;
	Advantage_view::Ptr m_disadvantages;
	Advantage_view::Ptr m_advantages;
	std::vector<Template> m_templates;
	Character_data* m_character;
	//Character_data m_temp_character;
	std::function<void(const std::string&)> m_refresh_template;
};

class Character_view : public Form
{
public:
	using Ptr = std::shared_ptr<Character_view>;

	Character_view();
	static Ptr create();
	sfg::Widget::Ptr widget() override;
	void commit(Character_data& character) override;
	void show(const Character_data& base, const Character_data& delta) const;
private:
	sfg::Widget::Ptr m_widget;
	Attribute_view::Ptr m_attribute_table;
	sfg::Table::Ptr m_talent_table;
	sfg::Table::Ptr m_disad_table;
	sfg::Table::Ptr m_advantage_table;
};

class Main_view
{
public:
	Main_view();
	sfg::Widget::Ptr widget();

	void set_phase(Phase phase);
	void commit(Character_data& character);
	void show_character_view(const Character_data& base, const Character_data& delta) const;
	void on_template_refresh(const std::string& template_name);
	inline void set_db(db::DB_connection::Ptr db) { m_db = db; }
private:
	void on_next();
	void commit_delta_to_base();

	db::DB_connection::Ptr m_db;
	sfg::Table::Ptr m_root;
	Character_view::Ptr m_character_view;
	sfg::Button::Ptr m_next_button;
	Form::Ptr m_current_form;
	Controller* m_controller;
	Character_data m_character_so_far;
	Character_data m_delta;
	Phase m_phase;
};

class State_character_generator : public State
{
public:
	explicit State_character_generator(Shared_context* context);

	void on_create() override;
	void on_destroy() override;
	void activate() override;
	void deactivate() override;
	void update(const sf::Time& time) override;
	void handle_sfml_event(sf::Event& e) override;
	void draw() override;

private:
	db::DB_connection::Ptr m_db;
	sfg::SFGUI m_sfgui;
	sfg::Desktop m_desktop;
	sfg::Window::Ptr m_gui_window;
	sfg::Widget::Ptr m_root_widget;
	Main_view m_view;
	Phase m_phase{ Phase::Name };
	int m_starting_cp;
};