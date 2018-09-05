#pragma once
#include <SFGUI/Widgets.hpp>
#include <unordered_map>
#include "db.h"

class Builder;
class Page
{
public:
	using Ptr = std::unique_ptr<Page>;
	virtual ~Page() = default;
	static void set_builder(Builder* builder) { m_builder = builder; }
	virtual sfg::Widget::Ptr get_page() = 0;
	virtual bool validate() = 0;
	virtual int insert_into_database(db::db_connection_ptr& db, const std::string& key) = 0;
	virtual std::string get_key() { return ""; }
protected:
	static Builder * m_builder;
private:
	
};

class Entity_page : public Page
{
	enum class Button { Shared, Projectile, Ranged };
public:
	using Ptr = std::unique_ptr<Entity_page>;
	static Ptr create() { return std::make_unique<Entity_page>(); }
	Entity_page();
	sfg::Widget::Ptr get_page() override { return m_page; }
	bool validate() override;
	int insert_into_database(db::db_connection_ptr& db, const std::string& key) override { return 0; }
private:
	void handle_button(Entity_page::Button button);
	std::string get_key() override { return m_name_entry->GetText(); }
	
	sfg::Widget::Ptr m_page;
	sfg::Entry::Ptr m_name_entry;
	std::unordered_map<Button, sfg::CheckButton::Ptr> m_buttons;
};

enum class Value_type { String, Integer, Float };

class Shared_page : public Page
{
public:
	
	Shared_page();
	using Ptr = std::unique_ptr<Shared_page>;
	static Ptr create() { return std::make_unique <Shared_page>(); }
	sfg::Widget::Ptr get_page() override { return m_page; }
	bool validate() override;
	int insert_into_database(db::db_connection_ptr& db, const std::string& key) override;
private:
	//void validate_text(Value_type type, sfg::Entry::Ptr entry);

	sfg::Widget::Ptr m_page;
	sfg::Entry::Ptr m_name_entry;
	sfg::Entry::Ptr m_description_entry;
	sfg::Entry::Ptr m_weight_entry;
	sfg::Entry::Ptr m_price_entry;
	sfg::Label::Ptr m_error_msg;
	db::prepared_statement_ptr m_insert_stmt = nullptr;
	db::prepared_statement_ptr m_max_index = nullptr;
};

class Projectile_page : public Page
{
public:
	Projectile_page();
	using Ptr = std::unique_ptr < Projectile_page>;
	static Ptr create() { return std::make_unique<Projectile_page>(); }
	sfg::Widget::Ptr get_page() override { return m_page; }
	bool validate() override;
	int insert_into_database(db::db_connection_ptr& db, const std::string& key) override;
private:
	std::vector<std::string> m_point_types = { "None", "Default", "Bodkin", "Cutting", "Flaming" };
	sfg::Label::Ptr m_error_msg;
	sfg::Widget::Ptr m_page;
	sfg::ComboBox::Ptr m_point_selector;
	sfg::Entry::Ptr m_damage_entry;
	sfg::Entry::Ptr m_range_entry;
	db::prepared_statement_ptr m_insert_stmt = nullptr;
	db::prepared_statement_ptr m_max_id = nullptr;
};

class Ranged_page : public Page
{
public:
	Ranged_page();
	using Ptr = std::unique_ptr<Ranged_page>;
	static Ptr create() { return std::make_unique<Ranged_page>(); }
	sfg::Widget::Ptr get_page() override { return m_page; }
	bool validate() override;
	int insert_into_database(db::db_connection_ptr& db, const std::string& key) override { return 0; }
private:
	std::vector<std::string> m_attack_types = { "Swing", "Thrust" };
	std::vector<std::string> m_damage_types = { "Crushing", "Cutting", "Impaling" };
	std::vector<std::string> m_projectiles = { "Arrow", "Bullet", "Bolt", "Thrown" };
	std::vector<std::string> m_ranged_skills = { "Blowpipe", "Bolas", "Bow", "Cloak", "Crossbow", "Lasso", "Net", "Sling", "Spear thrower",
												"Thrown Weapon (Axe/Mace)", "Thrown Weapon (Dart)", "Thrown Weapon (Knife)", "Thrown Weapon (Shuriken)",
												"Thrown Weapon (Spear)", "Thrown Weapon (Stick)" };
	sfg::Label::Ptr m_error_msg;
	sfg::Widget::Ptr m_page;
	sfg::ComboBox::Ptr m_skill;
	sfg::ComboBox::Ptr m_attack_type;
	sfg::ComboBox::Ptr m_damage_type;
	sfg::Entry::Ptr m_damage_bonus;
	sfg::ComboBox::Ptr m_projectile;
	sfg::Entry::Ptr m_accuracy;
	sfg::Entry::Ptr m_min_strength;
	sfg::CheckButton::Ptr m_two_handed;
	sfg::Entry::Ptr m_bulk;
	sfg::Entry::Ptr m_half_damage_range;
	sfg::Entry::Ptr m_max_range;
	sfg::Entry::Ptr m_nr_shots;
	sfg::Entry::Ptr m_reload_time;
};