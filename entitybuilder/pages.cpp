#include "stdafx.h"
#include "pages.h"
#include "builder.h"
#include <iostream>
#include <regex>
#include <variant>

Builder* Page::m_builder{ nullptr };

// enums must be equal to item.h
enum class Damage_type { cr, cut, imp, pi_, pi, spec, ranged, Default };
enum class Point { None, Default, Bodkin, Cutting, Flaming };

Entity_page::Entity_page()
{
	auto outer_box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	auto name_box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	outer_box->Pack(name_box, false, true);

	name_box->Pack(sfg::Label::Create("entity name"));
	m_name_entry = sfg::Entry::Create();
	m_name_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));
	name_box->Pack(m_name_entry, false, true);

	m_buttons[Button::Shared] = sfg::CheckButton::Create("Shared data");
	m_buttons[Button::Shared]->GetSignal(sfg::CheckButton::OnToggle).Connect([this]() {handle_button(Button::Shared); });

	m_buttons[Button::Projectile] = sfg::CheckButton::Create("Projectile data");
	m_buttons[Button::Projectile]->GetSignal(sfg::CheckButton::OnToggle).Connect([this]() {handle_button(Button::Projectile); });

	m_buttons[Button::Ranged] = sfg::CheckButton::Create("Ranged weapon");
	m_buttons[Button::Ranged]->GetSignal(sfg::CheckButton::OnToggle).Connect([this]() {handle_button(Button::Ranged); });

	outer_box->Pack(m_buttons[Button::Shared]);
	outer_box->Pack(m_buttons[Button::Projectile]);
	outer_box->Pack(m_buttons[Button::Ranged]);

	m_page = outer_box;
}

void Entity_page::handle_button(Button button)
{
	std::cout << "button toggled" << std::endl;
	bool status;
	m_buttons[button]->IsActive() ? status = true : status = false;
	Page_type type;
	switch (button)
	{
	case Entity_page::Button::Shared:
		type = Page_type::Shared;
		break;
	case Entity_page::Button::Projectile:
		type = Page_type::Projectile;
		break;
	case Entity_page::Button::Ranged:
		type = Page_type::Ranged;
		break;
	default:
		break;
	}
	m_builder->set_tab_status(type, status);
}

bool Entity_page::validate()
{
	return m_name_entry->GetText().getSize();
}

std::string validate_text(Value_type type, sfg::Entry::Ptr entry)
{
	std::string text = entry->GetText();
	std::string result;
	switch (type)
	{
	case Value_type::String:
		text.size() == 0 ? result = "Must not be empty" : result = "";
		//result = "";
		break;
	case Value_type::Integer:
		std::regex_match(text, std::regex("[0-9]+")) ? result = "" : result = "Must be a valid integer value";
		break;
	case Value_type::Float:
		std::regex_match(text, std::regex("[0-9]+([.][0-9]+)?")) ? result = "" : result = "Must be a valid floating point number";
		break;
	default:
		break;
	}
	return result;
}


Shared_page::Shared_page()
{
	m_error_msg = sfg::Label::Create("");
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	box->Pack(m_error_msg);
	auto table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);
	table->Attach(sfg::Label::Create("Name"), sf::Rect<sf::Uint32>(0, 0, 1, 1));
	table->Attach(sfg::Label::Create("Description"), sf::Rect<sf::Uint32>(0, 1, 1, 1));
	table->Attach(sfg::Label::Create("Price"), sf::Rect<sf::Uint32>(0, 2, 1, 1));
	table->Attach(sfg::Label::Create("Weight"), sf::Rect<sf::Uint32>(0, 3, 1, 1));

	m_name_entry = sfg::Entry::Create();
	m_description_entry = sfg::Entry::Create();
	m_price_entry = sfg::Entry::Create();
	m_weight_entry = sfg::Entry::Create();
	
	m_name_entry->GetSignal(sfg::Entry::OnTextChanged).Connect([this, &e = m_name_entry]() {m_error_msg->SetText( validate_text(Value_type::String, e)); });
	m_name_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));
	m_description_entry->GetSignal(sfg::Entry::OnTextChanged).Connect([this, &e = m_description_entry]() {m_error_msg->SetText( validate_text(Value_type::String, e)); });
	m_description_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));
	m_price_entry->GetSignal(sfg::Entry::OnTextChanged).Connect([this, &e = m_price_entry]() {m_error_msg->SetText( validate_text(Value_type::Integer, e)); });
	m_price_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));
	m_weight_entry->GetSignal(sfg::Entry::OnTextChanged).Connect([this, &e = m_weight_entry]() {m_error_msg->SetText( validate_text(Value_type::Float, e)); });
	m_weight_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));

	table->Attach(m_name_entry, sf::Rect<sf::Uint32>(1, 0, 1, 1));
	table->Attach(m_description_entry, sf::Rect<sf::Uint32>(1, 1, 1, 1));
	table->Attach(m_price_entry, sf::Rect<sf::Uint32>(1, 2, 1, 1));
	table->Attach(m_weight_entry, sf::Rect<sf::Uint32>(1, 3, 1, 1));

	box->Pack(table, false, true);
	m_page = box;
}


bool Shared_page::validate()
{
	if (m_name_entry->GetText().getSize() == 0) return false;
	if (m_description_entry->GetText().getSize() == 0) return false;
	if (m_price_entry->GetText().getSize() == 0 || validate_text(Value_type::Integer, m_price_entry) != "") return false;
	if (m_weight_entry->GetText().getSize() == 0 || validate_text(Value_type::Float, m_weight_entry) != "") return false;
	return true;
}

int Shared_page::insert_into_database(db::db_connection_ptr& db, const std::string& key)
{
	std::string sql_string = "insert into item_shared (name, description, price, weight, entity_key) values (?, ?, ?, ?, ?)";
	std::string sql_max_index = "select max(id) as max_id from item_shared";
	if (m_insert_stmt == nullptr)
	{
		m_insert_stmt = db->prepare(sql_string);
	}
	if (m_max_index == nullptr)
	{
		try 
		{
			m_max_index = db->prepare(sql_max_index);
		}
		catch (std::string& msg)
		{
			std::cout << msg << std::endl;
		}
	}
	m_insert_stmt->bind(1, m_name_entry->GetText().toAnsiString());
	m_insert_stmt->bind(2, m_description_entry->GetText().toAnsiString());
	auto x =  atoi (m_price_entry->GetText().toAnsiString().c_str());
	m_insert_stmt->bind(3, x);
	auto y = atof(m_weight_entry->GetText().toAnsiString().c_str());
	m_insert_stmt->bind(4, y);
	m_insert_stmt->bind(5, key);
	if (m_insert_stmt->execute_row() == db::Result_code::Success)
	{
		m_insert_stmt->reset();
		auto rc = m_max_index->execute_row();
		if (rc == db::Result_code::Row)  // success
		{
			auto data = m_max_index->fetch_row();
			m_max_index->reset();
			return std::get<int>(data["max_id"]);
		}
	}
	return 0;
}


Projectile_page::Projectile_page()
{
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	m_error_msg = sfg::Label::Create("");
	box->Pack(m_error_msg);
	auto table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);

	auto point_label = sfg::Label::Create("Point type");
	auto damage_label = sfg::Label::Create("Damage bonus");
	auto range_label = sfg::Label::Create("Range bonus");

	m_point_selector = sfg::ComboBox::Create();
	std::for_each(m_point_types.begin(), m_point_types.end(), [&p = m_point_selector](auto& label) { p->AppendItem(label); });
	m_point_selector->SelectItem(1);
	m_damage_entry = sfg::Entry::Create();
	m_damage_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));
	m_damage_entry->GetSignal(sfg::Entry::OnTextChanged).Connect([this]() {m_error_msg->SetText(validate_text(Value_type::Integer, m_damage_entry)); });
	m_damage_entry->SetText("0");
	m_range_entry = sfg::Entry::Create();
	m_range_entry->SetRequisition(sf::Vector2f(120.0f, 0.0f));
	m_range_entry->GetSignal(sfg::Entry::OnTextChanged).Connect([this]() {m_error_msg->SetText(validate_text(Value_type::Integer, m_range_entry)); });
	m_range_entry->SetText("0");

	table->Attach(point_label, sf::Rect<sf::Uint32>(0, 0, 1, 1));
	table->Attach(damage_label, sf::Rect<sf::Uint32>(0, 1, 1, 1));
	table->Attach(range_label, sf::Rect<sf::Uint32> (0, 2, 1, 1));

	table->Attach(m_point_selector, sf::Rect<sf::Uint32>(1, 0, 1, 1));
	table->Attach(m_damage_entry, sf::Rect<sf::Uint32>(1, 1, 1, 1));
	table->Attach(m_range_entry, sf::Rect<sf::Uint32>(1, 2, 1, 1));

	box->Pack(table, false, true);
	m_page = box;
}

bool Projectile_page::validate()
{
	if (m_damage_entry->GetText().getSize() == 0 || validate_text(Value_type::Integer, m_damage_entry) != "") return false;
	if (m_range_entry->GetText().getSize() == 0 || validate_text(Value_type::Integer, m_range_entry) != "") return false;
	return true;
}

int Projectile_page::insert_into_database(db::db_connection_ptr& db, const std::string& key)
{
	std::string sql_insert = "insert into projectile (point_type, damage_bonus, range_multiplier, damage_type, armour_divisor, entity_key) values (?, ?, ?, ?, ?, ?)";
	std::string sql_max_id = "select max(id) as max_id from projectile";
	if (m_insert_stmt == nullptr)
	{
		m_insert_stmt = db->prepare(sql_insert);
	}
	if (m_max_id == nullptr)
	{
		m_max_id = db->prepare(sql_max_id);
	}
	auto point = m_point_selector->GetSelectedItem();
	Damage_type damage_type;
	float armour_divisor = 1.0f;
	switch (static_cast <Point> (point))
	{
	case Point::None:   // sling bullet, use damage type from weapon 
		damage_type = Damage_type::Default;
		break;
	case Point::Default:  // standard broadhead point
		damage_type = Damage_type::imp;
		break;
	case Point::Bodkin:
		damage_type = Damage_type::pi;
		armour_divisor = 2.0f;
		break;
	case Point::Cutting:
		damage_type = Damage_type::cut;
		break;
	case Point::Flaming:
		damage_type = Damage_type::imp;
		break;
	}
	m_insert_stmt->bind(1, point);
	m_insert_stmt->bind(2, atoi(m_damage_entry->GetText().toAnsiString().c_str()));
	m_insert_stmt->bind(3, atoi(m_range_entry->GetText().toAnsiString().c_str()));
	m_insert_stmt->bind(4, static_cast<int>(damage_type));
	m_insert_stmt->bind(5, armour_divisor);
	m_insert_stmt->bind(6, key);
	auto rc = m_insert_stmt->execute_row();
	if (rc == db::Result_code::Success)
	{
		m_insert_stmt->reset();
		rc = m_max_id->execute_row();
		if (rc == db::Result_code::Row)
		{
			auto data = m_max_id->fetch_row();
			m_max_id->reset();
			return std::get<int>(data["max_id"]);
		}
	}
	return 0;
}

Ranged_page::Ranged_page()
{
	sfg::Box::Ptr outer = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	m_error_msg = sfg::Label::Create("");
	outer->Pack(m_error_msg);
	sfg::Table::Ptr table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);
	table->SetColumnSpacings(10.0f);

	sfg::Label::Ptr skill_label = sfg::Label::Create("skill used");
	sfg::Label::Ptr attack_type_label = sfg::Label::Create("attack type");
	sfg::Label::Ptr damage_type_label = sfg::Label::Create("damage type");
	sfg::Label::Ptr damage_bonus_label = sfg::Label::Create("damage bonus");
	sfg::Label::Ptr projectile_label = sfg::Label::Create("projectile type");
	sfg::Label::Ptr accuracy_label = sfg::Label::Create("accuracy");
	sfg::Label::Ptr min_strength_label = sfg::Label::Create("minimum strength");
	sfg::Label::Ptr two_handed_label = sfg::Label::Create("two-handed");
	sfg::Label::Ptr bulk_label = sfg::Label::Create("bulk");
	sfg::Label::Ptr nr_shots_label = sfg::Label::Create("number of shots");
	sfg::Label::Ptr reload_time_label = sfg::Label::Create("reload time");
	sfg::Label::Ptr half_damage_label = sfg::Label::Create("half damage");
	sfg::Label::Ptr max_range_label = sfg::Label::Create("max range");

	m_skill = sfg::ComboBox::Create();
	std::for_each(m_ranged_skills.begin(), m_ranged_skills.end(), [&p = m_skill](auto& label) { p->AppendItem(label); });
	m_skill->SelectItem(2);

	m_attack_type = sfg::ComboBox::Create();
	std::for_each(m_attack_types.begin(), m_attack_types.end(), [&p = m_attack_type](auto& label) { p->AppendItem(label); });
	m_attack_type->SelectItem(1);

	m_damage_type = sfg::ComboBox::Create();
	std::for_each(m_damage_types.begin(), m_damage_types.end(), [&p = m_damage_type](auto& label) { p->AppendItem(label); });
	m_damage_type->SelectItem(2);

	m_damage_bonus = sfg::Entry::Create();

	m_half_damage_range = sfg::Entry::Create();
	m_half_damage_range->SetRequisition(sf::Vector2f(0.0f, 10.0f));
	m_max_range = sfg::Entry::Create();
	m_max_range->SetRequisition(sf::Vector2f(0.0f, 10.0f));

	sfg::Frame::Ptr attack_frame = sfg::Frame::Create("Attack");
	sfg::Frame::Ptr range_frame = sfg::Frame::Create("Range");

	sfg::Table::Ptr attack_table = sfg::Table::Create();
	attack_table->SetRowSpacings(10.0f);
	attack_table->SetColumnSpacings(10.0f);
	attack_table->Attach(attack_type_label, sf::Rect<sf::Uint32>(0, 0, 1, 1));
	attack_table->Attach(damage_type_label, sf::Rect<sf::Uint32>(1, 0, 1, 1));
	attack_table->Attach(damage_bonus_label, sf::Rect<sf::Uint32>(2, 0, 1, 1));

	attack_table->Attach(m_attack_type, sf::Rect<sf::Uint32>(0, 1, 1, 1));
	attack_table->Attach(m_damage_type, sf::Rect<sf::Uint32>(1, 1, 1, 1));
	attack_table->Attach(m_damage_bonus, sf::Rect<sf::Uint32>(2, 1, 1, 1));

	attack_frame->Add(attack_table);

	sfg::Table::Ptr range_table = sfg::Table::Create();
	range_table->SetRowSpacings(10.0f);
	range_table->SetColumnSpacings(10.0f);

	range_table->Attach(half_damage_label, sf::Rect<sf::Uint32>(0, 0, 1, 1));
	range_table->Attach(max_range_label, sf::Rect<sf::Uint32>(1, 0, 1, 1));

	range_table->Attach(m_half_damage_range, sf::Rect<sf::Uint32>(0, 1, 1, 1));
	range_table->Attach(m_max_range, sf::Rect<sf::Uint32>(1, 1, 1, 1));

	range_frame->Add(range_table);

	m_projectile = sfg::ComboBox::Create();
	std::for_each(m_projectiles.begin(), m_projectiles.end(), [&p = m_projectile](auto& label) { p->AppendItem(label); });
	m_projectile->SelectItem(0);

	m_accuracy = sfg::Entry::Create();
	m_accuracy->SetRequisition(sf::Vector2f(0.0f, 10.0f));
	m_min_strength = sfg::Entry::Create();
	m_min_strength->SetRequisition(sf::Vector2f(0.0f, 10.0f));
	m_two_handed = sfg::CheckButton::Create("");
	m_bulk = sfg::Entry::Create();
	m_bulk->SetRequisition(sf::Vector2f(0.0f, 10.0f));
	m_nr_shots = sfg::Entry::Create();
	m_nr_shots->SetRequisition(sf::Vector2f(0.0f, 10.0f));
	m_reload_time = sfg::Entry::Create();
	m_reload_time->SetRequisition(sf::Vector2f(0.0f, 10.0f));

	table->Attach(skill_label, sf::Rect<sf::Uint32>(0, 0, 1, 1));
	table->Attach(projectile_label, sf::Rect<sf::Uint32>(0, 2, 1, 1));
	table->Attach(accuracy_label, sf::Rect<sf::Uint32>(0, 3, 1, 1));
	table->Attach(min_strength_label, sf::Rect<sf::Uint32>(0, 4, 1, 1));
	table->Attach(two_handed_label, sf::Rect<sf::Uint32>(0, 5, 1, 1));
	table->Attach(bulk_label, sf::Rect<sf::Uint32>(0, 6, 1, 1));
	table->Attach(nr_shots_label, sf::Rect<sf::Uint32>(0, 7, 1, 1));
	table->Attach(reload_time_label, sf::Rect<sf::Uint32>(0, 8, 1, 1));
	//table->Attach(attack_frame, sf::Rect<sf::Uint32>(0, 7, 3, 1));

	table->Attach(m_skill, sf::Rect<sf::Uint32>(1, 0, 1, 1));
	table->Attach(m_projectile, sf::Rect<sf::Uint32>(1, 2, 1, 1));
	table->Attach(m_accuracy, sf::Rect<sf::Uint32>(1, 3, 1, 1));
	table->Attach(m_min_strength, sf::Rect<sf::Uint32>(1, 4, 1, 1));
	table->Attach(m_two_handed, sf::Rect<sf::Uint32>(1, 5, 1, 1));
	table->Attach(m_bulk, sf::Rect<sf::Uint32>(1, 6, 1, 1));
	table->Attach(m_nr_shots, sf::Rect<sf::Uint32>(1, 7, 1, 1));
	table->Attach(m_reload_time, sf::Rect<sf::Uint32>(1, 8, 1, 1));

	outer->Pack(table, false, false);
	outer->Pack(attack_frame, false, true);
	outer->Pack(range_frame, false, true);
	m_page = outer;
}

bool Ranged_page::validate()
{
	if (validate_text(Value_type::Integer, m_accuracy) != "") return false;
}