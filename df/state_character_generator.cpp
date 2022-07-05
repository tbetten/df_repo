#include "state_character_generator.h"
#include <ranges>
#include <numeric>
#include <format>
#include <assert.h>
#include <stdexcept>

Template_type string_to_template_type(const std::string& str)
{
	if (str == "Race") return Template_type::Race;
	if (str == "Occupation") return Template_type::Occupation;
	return Template_type::Invalid;
}

Template::Template(db::DB_connection::Ptr db, const std::string& name)
{
	auto stmt = db->prepare("select name, is_default, type, cost, max_disads from template where name = ?");
	stmt.bind(1, name);
	stmt.execute_row();
	auto row = stmt.fetch_row();
	m_name = db::as_string(row["name"]);
	m_default = db::as_bool(row["is_default"]);
	m_type = string_to_template_type (db::as_string(row["type"]));
	m_cost = db::as_int(row["cost"]);
	m_max_disad_cp = db::is_null(row["max_disads"]) ? 0 : static_cast<unsigned int>(db::as_int(row["max_disads"]));
	read_attrib_modifiers(db);
	read_advantages(db);
	read_talents(db);
}

std::vector<attributes::Transaction> modifier_to_transactions(Attrib_modifier modifier, attributes::Template_type template_type, const std::string& template_name)
{
	std::vector<attributes::Transaction> transactions;
	auto attribute = modifier.m_attribute;
	auto cost = (modifier.m_amount / 100) * attributes::points_per_unit(attribute);
	transactions.emplace_back(attribute, attributes::Transaction_type::Buy, template_type, template_name, modifier.m_amount * 100, cost, std::nullopt);
	auto raises = attributes::raises_base(attribute);
	for (auto [attribute, amount] : raises)
	{
		transactions.emplace_back(attribute, attributes::Transaction_type::Raise_base, template_type, template_name, modifier.m_amount * amount, 0, std::nullopt);
	}
	return transactions;
}

void Template::read_attrib_modifiers(db::DB_connection::Ptr db)
{
	auto stmt = db->prepare("select attribute, modifier_min, modifier_max from template_attribute where template = ?");
	stmt.bind(1, m_name);
	auto modifiers = stmt.fetch_table();
	for (auto& modifier : modifiers)
	{
		auto attribute = attributes::string_to_attrib(db::as_string(modifier["attribute"]));
		auto min_amount = db::as_int(modifier["modifier_min"]) * 100;
		auto max_amount = db::as_int(modifier["modifier_max"]) * 100;
		m_modifiers.emplace_back(attribute, min_amount, max_amount, min_amount);
	}
}

void Template::read_talents(db::DB_connection::Ptr db)
{
	auto stmt = db->prepare("select name, max_levels, standard_level, cost_per_level, description, upgradeable from talent inner join talent_template on talent_template.Talent = talent.name where talent_template.Template = ?");
	stmt.bind(1, m_name);
	auto rows = stmt.fetch_table();
	for (auto& row : rows)
	{
		auto name = db::as_string(row["name"]);
		auto description = db::as_string(row["description"]);
		auto max_level = db::as_int(row["max_levels"]) * 100;
		auto min_level = db::as_int(row["standard_level"]) * 100;
		auto upgradeable = db::as_bool(row["upgradeable"]);
		auto cost_per_level = db::as_int(row["cost_per_level"]);
		m_talents.emplace_back(name, description, upgradeable, min_level, min_level, max_level, cost_per_level);

		auto skill_stmt = db->prepare("select skill from talent_skill where talent = ?");
		skill_stmt.bind(1, name);
		auto skills = skill_stmt.fetch_table();
		for (auto& talent = m_talents.back(); auto & skill : skills)
		{
			talent.m_skills.emplace_back(db::as_string(skill["skill"]));
		}
	}
}

void Template::read_advantages(db::DB_connection::Ptr db)
{
	auto sc_stmt = db->prepare("select key, name, perc, multiplier from selfcontrol_levels");
	auto selfcontrol_levels = sc_stmt.fetch_table();
	for (auto& selfcontrol_level : selfcontrol_levels)
	{
		Selfcontrol sc;
		sc.key = db::as_int(selfcontrol_level["key"]);
		sc.m_name = db::as_string(selfcontrol_level["name"]);
		sc.perc = db::as_int(selfcontrol_level["perc"]);
		sc.multiplier = db::as_double(selfcontrol_level["multiplier"]);
		m_selfcontrol_levels.push_back(sc);
	}

	auto stmt = db->prepare("select advantage, default_level, max_level, selfcontrol, cost_per_level, description, mandatory from advantage_template join advantage on advantage_template.advantage = advantage.name where template_name = ?");
	stmt.bind(1, m_name);
	auto advantages = stmt.fetch_table();
	for (auto& advantage : advantages)
	{
		Advantage a;
		a.m_name = db::as_string(advantage["advantage"]);
		a.m_description = db::as_string(advantage["description"]);
		a.m_default_level = db::as_int(advantage["default_level"]);
		a.m_max_level = db::as_int(advantage["max_level"]);
		a.m_level = a.m_default_level;
		a.m_cost_per_level = db::as_int(advantage["cost_per_level"]);
		a.m_cost = a.m_cost_per_level * a.m_default_level;
		if (!db::is_null(advantage["selfcontrol"]))
		{
			auto sc = db::as_int(advantage["selfcontrol"]);
			auto itr = std::ranges::find_if(m_selfcontrol_levels, [sc](const Selfcontrol& selfcontrol) {return sc == selfcontrol.key; });
			if (itr == std::cend(m_selfcontrol_levels)) throw std::out_of_range(std::format("unknown key for selfcontrol level {}", sc));
			a.m_self_control = itr->perc;
		}
		else
		{
			a.m_self_control = std::nullopt;
		}
		a.m_mandatory = db::as_bool(advantage["mandatory"]);
		if (a.m_cost_per_level < 0)
		{
			m_disadvantages.push_back(a);
		}
		else
		{
			m_advantages.push_back(a);
		}
	}
}


// view

Main_view::Main_view()
{


	auto table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);
	table->SetColumnSpacings(30.0f);
	m_root = table;
	m_next_button = sfg::Button::Create("Next");
	m_next_button->GetSignal(sfg::Button::OnLeftClick).Connect([this]() {on_next(); });
	table->Attach(m_next_button, { 1,1,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	m_character_view = Character_view::create();
	table->Attach(m_character_view->widget(), { 1,0,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
}

sfg::Widget::Ptr Main_view::widget()
{
	return m_root;
}

void Main_view::set_phase(Phase phase)
{
	m_phase = phase;
	auto old_form = find_widget<sfg::Widget>("form");
	m_root->Remove(old_form);
	old_form.reset();
	Form::Ptr form;
	
	switch (phase)
	{
	case Phase::Name:
		form = Name_form::create();
		m_current_form = form;
		break;
	case Phase::Race:
		[[fallthrough]];
	case Phase::Occupation:
		form = Template_form::create(phase, m_db, [this](const std::string& template_name) {on_template_refresh(template_name); }, &m_delta);
		m_current_form = form;
		break;
	}
	auto widget = form->widget();
	widget->SetId("form");
	m_root->Attach(widget, { 0,0,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
}

void Main_view::commit(Character_data& character)
{
	m_current_form->commit(character);
}

void Main_view::show_character_view(const Character_data& base, const Character_data& delta) const
{
	m_character_view->show(base, delta);
}

void Main_view::on_template_refresh(const std::string& template_name)
{
	std::cout << "template refresh " << template_name << "\n";
	commit(m_delta);
	show_character_view(m_character_so_far, m_delta);
}

Name_form::Name_form()
{
	auto table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);
	table->SetColumnSpacings(20.0f);
	auto name_label = sfg::Label::Create("Name");
	auto name_entry = sfg::Entry::Create();
	auto male = sfg::RadioButton::Create("Male");
	auto female = sfg::RadioButton::Create("Female", male->GetGroup());
	male->SetActive(true);
	name_entry->SetId("name_entry");
	male->SetId("male_rb");
	table->Attach(name_label, { 0,0,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(name_entry, { 1,0,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(male, { 0,1,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(female, { 1,1,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	m_widget = table;
}

Form::Ptr Name_form::create()
{
	return std::make_shared<Name_form>();
}

sfg::Widget::Ptr Name_form::widget()
{
	return m_widget;
}

void Name_form::commit(Character_data& character)
{
	character.m_name = find_widget<sfg::Entry>("name_entry")->GetText();
	character.m_gender = find_widget<sfg::RadioButton>("male_rb")->IsActive() ? Gender::Male : Gender::Female;
}

Attribute_view::Attribute_view(std::function<void(const std::string&)> refresh_attributes, std::string template_name) : m_refresh_attributes{ refresh_attributes }, m_template_name{ std::move(template_name) }
{
	m_widget = sfg::Table::Create();
	m_widget->SetRowSpacings(10.0f);
	m_widget->SetColumnSpacings(20.0f);
}

Attribute_view::Ptr Attribute_view::create(std::function<void(const std::string&)> refresh_attributes, std::string template_name)
{
	return std::make_shared<Attribute_view>(refresh_attributes, template_name);
}

sfg::Widget::Ptr Attribute_view::widget()
{
	return m_widget;
}

void Attribute_view::clear()
{
	for (auto [attribute, label_ptr] : m_value_labels)
	{
		label_ptr->SetText("");
	}
}

std::tuple<int, int> attribute_values(std::span<attributes::Transaction> transactions, attributes::Attrib attribute)
{
	std::vector<attributes::Transaction> transactions_for_attrib{};
	std::ranges::copy_if(transactions, std::back_inserter(transactions_for_attrib), [attribute](const attributes::Transaction& transaction) {return transaction.attribute == attribute; });
	auto amount = attributes::get_total_value(transactions_for_attrib, attribute);
	auto cost = std::accumulate(transactions_for_attrib.begin(), transactions_for_attrib.end(), 0, [](int points_so_far, const attributes::Transaction& transaction) {return points_so_far + transaction.points_spent; });
	return std::make_tuple(amount, cost);
}

void attach_attribute(sfg::Table::Ptr table, sf::Uint32 column, sf::Uint32 row, const sfg::Label::Ptr& label, const sfg::Label::Ptr& amount, const sfg::Label::Ptr& cost)
{
	table->Attach(label, { column, row, 1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(amount, { column + 1, row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(cost, { column + 2,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
}

void Attribute_view::show(const std::span<attributes::Transaction> transactions)
{
	m_widget->RemoveAll();
	sf::Uint32 row{ 0 };
	for (auto attribute : attributes::primary_attributes)
	{
		auto [amount, cost] = attribute_values(transactions, attribute);
		attach_attribute(m_widget, 0, row, sfg::Label::Create(attributes::attrib_to_string(attribute)), sfg::Label::Create(std::format("{:.2f}",amount / 100.0f)), sfg::Label::Create("[" + std::to_string(cost) + "]"));
		++row;
	}
	row = 0;
	for (auto attribute : attributes::secundary_attributes)
	{
		auto [amount, cost] = attribute_values(transactions, attribute);
		attach_attribute(m_widget, 3, row, sfg::Label::Create(attributes::attrib_to_string(attribute)), sfg::Label::Create(std::format("{:.2f}",amount / 100.0f)), sfg::Label::Create("[" + std::to_string(cost) + "]"));
		++row;
	}
	++row;
	for (auto attribute : attributes::tertiary_attributes)
	{
		auto [amount, cost] = attribute_values(transactions, attribute);
		attach_attribute(m_widget, 0, row, sfg::Label::Create(attributes::attrib_to_string(attribute)), sfg::Label::Create(std::format("{:.2f}", amount / 100.0f)), sfg::Label::Create("[" + std::to_string(cost) + "]"));
		++row;
	}

	auto total = std::accumulate(std::cbegin(transactions), std::cend(transactions), 0, [](int so_far, const attributes::Transaction& trans) {return so_far + trans.points_spent; });
	m_widget->Attach(sfg::Label::Create(std::format("Total CP: [{}]", total)), { 0,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	
}

void Attribute_view::on_scale_changed(sfg::Scale* scale, sfg::Label* amount, sfg::Label* cost, int ppu, Attrib_modifier* modifier_ptr)
{
	auto cost_num = scale->GetValue();
	auto amount_num = cost_num / ppu;
	amount->SetText(std::format("{:.2f}", amount_num));
	cost->SetText("[" + std::format ("{}",cost_num) + "]");
	modifier_ptr->m_amount = static_cast<int>(amount_num * 100);
	m_refresh_attributes(m_template_name);
}

void Attribute_view::attach_modified_attribute(sf::Uint32 column, sf::Uint32 row, attributes::Attrib attribute, Attrib_modifier* modifier)
{
	sfg::Label::Ptr cost_label=sfg::Label::Create("");
	sfg::Label::Ptr amount_label=sfg::Label::Create("");
	auto s = attributes::attrib_to_string(attribute);
	auto l = sfg::Label::Create(attributes::attrib_to_string(attribute));
	if (modifier)
	{
		auto mod = *modifier;
		amount_label->SetText(std::to_string(mod.m_amount / 100));
		auto ppu = attributes::points_per_unit(attribute);
		cost_label->SetText("[" + std::to_string(ppu * (mod.m_amount / 100)) + "]");
		if (mod.m_min_amount != mod.m_max_amount)
		{
			auto scale = sfg::Scale::Create(static_cast<float>(mod.m_min_amount * ppu / 100), static_cast<float>(mod.m_max_amount * ppu / 100), 1.0f);
			scale->SetRequisition({ 50.0f, 25.0f });
			auto scale_ptr = scale.get();
			auto amount_ptr = amount_label.get();
			auto cost_ptr = cost_label.get();
			scale->GetAdjustment()->GetSignal(sfg::Adjustment::OnChange).Connect([this, scale_ptr, amount_ptr, cost_ptr, ppu, modifier]() {on_scale_changed(scale_ptr, amount_ptr, cost_ptr, ppu, modifier); });;
			m_widget->Attach(scale, { column + 3,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		}
	}
	attach_attribute(m_widget, column, row, sfg::Label::Create(attributes::attrib_to_string(attribute)), amount_label, cost_label);
}

void Attribute_view::show(std::span<Attrib_modifier> modifiers)
{
	m_widget->RemoveAll();
	sf::Uint32 row{ 0 };
	
	for (auto& primary : attributes::primary_attributes)
	{
		Attrib_modifier* modifier_ptr{ nullptr };
		auto itr = std::ranges::find_if(modifiers, [primary](const Attrib_modifier& modifier) {return modifier.m_attribute == primary; });
		if (itr != std::end(modifiers)) modifier_ptr = &(* itr);
		attach_modified_attribute( 0, row, primary, modifier_ptr);
		++row;
	}
	row = 0;
	
	for (auto& secundary : attributes::secundary_attributes)
	{
		Attrib_modifier* modifier_ptr{ nullptr };
		auto itr = std::ranges::find_if(modifiers, [secundary](const Attrib_modifier& modifier) {return modifier.m_attribute == secundary; });
		if (itr != std::end(modifiers)) modifier_ptr = &(*itr);
		attach_modified_attribute( 4, row, secundary, modifier_ptr);
		++row;
	}
	++row;
	for (auto& tertiary : attributes::tertiary_attributes)
	{
		Attrib_modifier* modifier_ptr{ nullptr };
		auto itr = std::ranges::find_if(modifiers, [tertiary](const Attrib_modifier& modifier) {return modifier.m_attribute == tertiary; });
		if (itr != std::end(modifiers)) modifier_ptr = &(*itr);
		attach_modified_attribute(0, row, tertiary, modifier_ptr);
		++row;
	}
}

Talent_view::Talent_view(std::function<void(const std::string&)> refresh_callback, const std::string& template_name) : m_refresh_callback{ refresh_callback }
{
	m_widget = sfg::Table::Create();
	m_widget->SetRowSpacings(10.0f);
	m_widget->SetColumnSpacings(20.0f);

	m_widget->Attach(sfg::Label::Create("talent"), { 0,0,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
}

Talent_view::Ptr Talent_view::create(std::function<void(const std::string&)> refresh_callback, const std::string& template_name)
{
	return std::make_shared<Talent_view>(refresh_callback, template_name);
}

sfg::Widget::Ptr Talent_view::widget()
{
	return m_widget;
}

void Talent_view::clear()
{
	m_widget->RemoveAll();
}

void Talent_view::on_scale_changed(sfg::Scale* scale, sfg::Label* value, sfg::Label* cost, Talent* talent)
{
	auto cost_val = scale->GetValue();
	auto ppu = talent->m_cost_per_level;
	auto value_val = cost_val / ppu;
	talent->m_value = static_cast<unsigned int>(value_val * 100);
	value->SetText(std::format("{:.2f}", value_val));
	cost->SetText(std::format("[{}]", static_cast<unsigned int>(cost_val)));
	m_refresh_callback("bla");
}

void Talent_view::show(std::span<Talent> talents)
{
	for (auto& talent : talents)
	{
		m_widget->Attach(sfg::Label::Create(talent.m_name), { 0,0,3,1 }, sfg::Table::FILL, sfg::Table::FILL);
		m_widget->Attach(sfg::Label::Create(talent.m_description), { 0,1,3,1 }, sfg::Table::FILL, sfg::Table::FILL);
		auto num_skills = talent.m_skills.size();
		m_widget->Attach(sfg::Label::Create("This talent gives bonuses to the following skills:"), { 0,2,1,num_skills }, sfg::Table::FILL, sfg::Table::FILL);
		sf::Uint32 line{ 3 };
		for (auto& skill : talent.m_skills)
		{
			m_widget->Attach(sfg::Label::Create(skill), { 2,line,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
			++line;
		}
		sfg::Label::Ptr value_label = sfg::Label::Create(std::to_string(talent.m_value / 100));
		sfg::Label::Ptr cost_label = sfg::Label::Create("[" + std::to_string(talent.m_cost_per_level * talent.m_value / 100) + "]");
		auto min_cost = static_cast<float>(talent.m_min_value * talent.m_cost_per_level / 100);
		auto max_cost = static_cast<float>(talent.m_max_value * talent.m_cost_per_level / 100);
		sfg::Scale::Ptr scale = sfg::Scale::Create(min_cost, max_cost, 1.0f);

		auto scale_ptr = scale.get();
		auto amount_ptr = value_label.get();
		auto cost_ptr = cost_label.get();
		auto ppu = talent.m_cost_per_level;
		auto talent_ptr = &talent;
		scale->GetAdjustment()->GetSignal(sfg::Adjustment::OnChange).Connect([this, scale_ptr, amount_ptr, cost_ptr, ppu, talent_ptr]() {on_scale_changed(scale_ptr, amount_ptr, cost_ptr, talent_ptr); });;

		m_widget->Attach(value_label, { 0,num_skills + 3,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		m_widget->Attach(cost_label, { 1,num_skills + 3,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		m_widget->Attach(scale, { 2,num_skills + 3,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	}
}

Advantage_view::Advantage_view(std::function<void(const std::string&)> refresh_callback, const std::string& template_name) : m_refresh_callback{ refresh_callback }
{
	m_widget = sfg::Table::Create();
	m_widget->SetRowSpacings(10.0f);
	m_widget->SetColumnSpacings(20.0f);
}

Advantage_view::Ptr Advantage_view::create(std::function<void(const std::string&)> refresh_callback, const std::string& template_name)
{
	return std::make_shared<Advantage_view>(refresh_callback, template_name);
}

sfg::Widget::Ptr Advantage_view::widget()
{
	return m_widget;
}

void Advantage_view::clear()
{
	m_widget->RemoveAll();
}
int Advantage_view::count_adv_cp(std::span<Advantage> advantages)
{
	auto total = std::accumulate(std::cbegin(m_mandatory_advantages), std::cend(m_mandatory_advantages), 0, [this, advantages](int so_far, size_t index) {return so_far + advantages[index].m_cost; });
	total = std::accumulate(std::cbegin(m_optional_advantages), std::cend(m_optional_advantages), total, [this, advantages](int so_far, advantage_pair ap) 
		{
			int delta{ 0 };
			if (auto rb = ap.first.lock())
			{
				delta = rb->IsActive() ? advantages[ap.second].m_cost : 0;
			}
/*			auto x = ap.first;
			if (auto y = x.lock())
			{
				auto delta = y->IsActive() ? advantages[ap.second].m_cost : 0;
			}
			auto delta = ap.first->IsActive() ? advantages[ap.second].m_cost : 0;*/
			return so_far + delta;
		});
	return total;
}


void Advantage_view::show(std::span<Advantage> advantages, const Template& current_template)
{
	sf::Uint32 row{ 1 };
	size_t index{ 0 };
	auto max_disads = current_template.m_max_disad_cp;
	for (auto advantage : advantages)
	{
		if (max_disads != 0)
		{
			m_widget->Attach(sfg::Label::Create(std::format("Pick -{} cp as disadvantages", max_disads)), { 0,0,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		}
		if (!advantage.m_mandatory)
		{
			auto check_button = sfg::CheckButton::Create("");
			check_button->GetSignal(sfg::CheckButton::OnToggle).Connect([this, advantages, index, max_disads]() {on_toggle(advantages, index, max_disads); });
			m_widget->Attach(check_button, {0,row,1,1}, sfg::Table::FILL, sfg::Table::FILL);
			m_optional_advantages.push_back(std::make_pair(check_button, index));
		}
		else
		{
			m_mandatory_advantages.push_back(index);
		}
		m_widget->Attach(sfg::Label::Create(advantage.m_name), {0,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		if (advantage.m_self_control)
		{
			auto sc = *advantage.m_self_control;
			auto itr = std::ranges::find_if(current_template.m_selfcontrol_levels, [sc](const Selfcontrol& level) {return sc == level.perc; });
			if (itr == std::cend(current_template.m_selfcontrol_levels)) throw std::out_of_range(std::format("unknown selfcontrol key {}", sc));
			auto level_index = std::distance(std::cbegin(current_template.m_selfcontrol_levels), itr);
			auto combobox = sfg::ComboBox::Create();
			for (auto& level : current_template.m_selfcontrol_levels)
			{
				combobox->AppendItem(level.m_name);
			}
			combobox->SelectItem(level_index);
			auto name = advantage.m_name;
			combobox->SetId(name);
			auto sub = advantages.subspan(index, 1);
			combobox->GetSignal(sfg::ComboBox::OnSelect).Connect([this, name, &levels = current_template.m_selfcontrol_levels, sub]() {on_selfcontrol_change(name, levels, sub); });
			m_widget->Attach(combobox, { 2,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		}
		auto cost_label = sfg::Label::Create(std::format("[{}]", advantage.m_cost_per_level));
		cost_label->SetId("l_" + advantage.m_name);
		m_widget->Attach(cost_label, { 3,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		++row;
		++index;
	}
}

void Advantage_view::on_selfcontrol_change(const std::string& name, const std::vector<Selfcontrol>& levels, std::span<Advantage> advantage)
{
	std::cout << "sc change " << name <<"\n";
	auto cb = find_widget<sfg::ComboBox>(name);
	auto val = cb->GetSelectedText().toAnsiString();
	std::cout << "new val: " << val << "\n";
	auto itr = std::ranges::find_if(levels, [val](const Selfcontrol& sc) {return sc.m_name == val; });
	auto mult = itr->multiplier;
	
	auto adv = advantage.front();
	auto cost = std::trunc(adv.m_cost_per_level * mult);
	std::cout << std::format("{}  {:.2f}  {:.2f}", adv.m_cost_per_level, mult, cost) << "\n";
	std::cout << std::format("{}%\n", itr->perc);
	auto cost_label = find_widget("l_" + name);
	cost_label->SetText(std::format("[{}]", cost));
	advantage[0].m_cost = cost;
	advantage[0].m_self_control = itr->perc;
	m_refresh_callback("");
}

void Advantage_view::on_toggle(std::span<Advantage> advantages, size_t index, unsigned int max_disads)
{
	std::cout << "toggle " << advantages[index].m_name << "\n";
	auto total = count_adv_cp(advantages);
	std::cout << total << "\n";

	if (std::abs(total) > max_disads)
	{
		auto itr = std::ranges::find_if(m_optional_advantages, [index](const advantage_pair& ap) {return ap.second == index; });
			if (itr != std::cend(m_optional_advantages))
			{
				if (auto rb = itr->first.lock()) rb->SetActive(false);
//				itr->first->SetActive(false);
			}
	}
}

std::vector<Template> get_templates(Phase phase, db::DB_connection::Ptr db)
{
	auto template_sql = "select name from template where type = ?";
	auto template_stmt = db->prepare(template_sql);
	switch (phase)
	{
	case Phase::Name:
		return std::vector<Template>{}; // should not happen
	case Phase::Race:
		template_stmt.bind(1, "Race");
		break;
	case Phase::Occupation:
		template_stmt.bind(1, "Occupation");
		break;
	}
	std::vector<Template> templates;
	auto template_rows = template_stmt.fetch_table();
	for (auto& template_row : template_rows)
	{
		auto template_name = db::as_string(template_row["name"]);
		templates.emplace_back(db, template_name);
	}
	return templates;
}

attributes::Template_type convert_template_type(Template_type type)
{
	switch (type)
	{
	case Template_type::Invalid: [[fallthrough]];
	case Template_type::None:
		return attributes::Template_type::Invalid;
	case Template_type::Race:
		return attributes::Template_type::Race;
	case Template_type::Occupation:
		return attributes::Template_type::Occupation;
	}
	return attributes::Template_type::Invalid;
}

int count_cp(const Template& character_template);

void Template_form::on_template_toggle(const size_t index)
{
	static bool ignore{ false };  // prevent endless firing of signals if template is too expensive
	if (ignore) return;
	m_current_template_index = index;
	auto& current_template = m_templates[index];
	m_current_template = current_template.m_name;
	auto rb = find_widget<sfg::RadioButton>(m_current_template);
	if (rb->IsActive())
	{
		m_current_rb = rb;
	}
	else
	{
		m_previous_rb = rb;
		return;
	}
	

	int cp = count_cp(current_template);
	if (cp > 250)  // TODO waarde uit db hierheen propageren
	{
		ignore = true;
		m_current_rb->SetActive(false);
		m_previous_rb->SetActive(true);
		ignore = false;
		return;
	}
	commit(*m_character);

	current_template.m_attribute_transactions.clear();

	m_attribute_table->clear();
	m_attribute_table->show(current_template.m_modifiers);

	m_talent_table->clear();
	m_talent_table->show(current_template.m_talents);

	m_disadvantages->clear();
	m_disadvantages->show(current_template.m_disadvantages, current_template);

	m_advantages->clear();
	m_advantages->show(current_template.m_advantages, current_template);
	m_refresh_template(m_current_template);
}

sfg::Widget::Ptr Template_form::create_template_table() 
{
	auto template_table = sfg::Table::Create();
	template_table->SetRowSpacings(10.0f);
	template_table->SetColumnSpacings(20.0f);
	size_t count{ 0 };
	std::shared_ptr<sfg::RadioButtonGroup> group{ nullptr };
	for (auto& template_rec : m_templates)
	{
		auto name = template_rec.m_name;
		auto template_rb = sfg::RadioButton::Create(name);
		template_rb->SetId(name);
		if (template_rec.m_default)
		{
			template_rb->SetActive(true);
			m_current_template = name;
		}
		if (!group) group = template_rb->GetGroup();
		template_rb->SetGroup(group);
		template_rb->GetSignal(sfg::RadioButton::OnToggle).Connect([this, count]() {on_template_toggle(count); });
		template_table->Attach(template_rb, { count % 2, count / 2,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
		++count;
	}
	return template_table;
}

Template_form::Template_form(Phase phase, db::DB_connection::Ptr db, std::function<void(const std::string&)> refresh_template_callback, Character_data* character) : m_refresh_template{ refresh_template_callback }, m_character{ character }
{
	m_templates = get_templates(phase, db);
	auto default_template_itr = std::ranges::find_if(m_templates, [](Template temp) {return temp.m_default; });
	size_t default_index{ 0 };  // index of default template
	if (default_template_itr != std::cend(m_templates))
	{
		default_index = std::distance(std::begin(m_templates), default_template_itr);
	}
	else
	{
		default_template_itr = std::begin(m_templates);
	}

	auto outer_table = sfg::Table::Create();
	outer_table->SetRowSpacings(10.0f);
	m_widget = outer_table;

	outer_table->Attach(create_template_table(), {0,0,1,1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	m_attribute_table = Attribute_view::create(m_refresh_template, m_current_template);
	m_talent_table = Talent_view::create(m_refresh_template, m_current_template);
	m_disadvantages = Advantage_view::create(m_refresh_template, m_current_template);
	m_advantages = Advantage_view::create(m_refresh_template, m_current_template);

	auto attribute_frame = sfg::Frame::Create("Attribute modifiers");
	attribute_frame->Add(m_attribute_table->widget());
	outer_table->Attach(attribute_frame, { 0,1,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	auto talent_frame = sfg::Frame::Create("Talents");
	talent_frame->Add(m_talent_table->widget());
	outer_table->Attach(talent_frame, { 0,2,1,1 }, sfg::Table::FILL, sfg::Table::FILL);

	auto disad_frame = sfg::Frame::Create("Disadvantages");
	disad_frame->Add(m_disadvantages->widget());
	outer_table->Attach(disad_frame, { 0,3,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	
	auto advantage_frame = sfg::Frame::Create("Advantages");
	advantage_frame->Add(m_advantages->widget());
	outer_table->Attach(advantage_frame, { 0,4,1,1 }, sfg::Table::FILL, sfg::Table::FILL);

	auto def = find_widget<sfg::RadioButton>(default_template_itr->m_name);
	def->SetActive(true);
}

Form::Ptr Template_form::create(Phase phase, db::DB_connection::Ptr db, std::function<void(const std::string&)> refresh_template_callback, Character_data* character)
{
	return std::make_shared<Template_form>(phase, db, refresh_template_callback, character);
}

sfg::Widget::Ptr Template_form::widget()
{
	return m_widget;
}

void Template_form::commit(Character_data& character)
{
	std::cout << "committing to character with name " << character.m_name << "\n";
	auto& current_template = m_templates[m_current_template_index];
	std::cout << "template is " << current_template.m_name << "\n";
	switch (current_template.m_type)
	{
	case Template_type::Race:
		character.m_race = current_template.m_name;
		break;
	case Template_type::Occupation:
		character.m_occupation = current_template.m_name;
		break;
	default:
		break;
	}
	character.m_attribute_transactions.clear();
	for (auto& modifier : current_template.m_modifiers)
	{
		auto cost = attributes::points_per_unit(modifier.m_attribute) * modifier.m_amount / 100;
		auto transactions = modifier_to_transactions(modifier.m_attribute, attributes::Transaction_type::Buy, convert_template_type(current_template.m_type), current_template.m_name, modifier.m_amount, cost, std::nullopt);
		std::ranges::copy(transactions, std::back_inserter(character.m_attribute_transactions));
	}
	character.m_talents.clear();
	std::ranges::copy(current_template.m_talents, std::back_inserter(character.m_talents));
	
	character.m_disadvantages.clear();
	character.m_advantages.clear();
	std::ranges::copy(current_template.m_disadvantages, std::back_inserter(character.m_disadvantages));
	std::ranges::copy(current_template.m_advantages, std::back_inserter(character.m_advantages));

}

Character_view::Character_view()
{
	auto table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);
	table->SetColumnSpacings(20.0f);
	auto name_label = sfg::Label::Create("");
	auto gender_label = sfg::Label::Create("");
	name_label->SetId("name_label");
	gender_label->SetId("gender_label");
	auto race_label = sfg::Label::Create("");
	race_label->SetId("race_label");
	auto occupation_label = sfg::Label::Create("");
	occupation_label->SetId("occupation_label");
	auto cp_label = sfg::Label::Create("");
	cp_label->SetId("cp_label");
	table->Attach(sfg::Label::Create("Name:"), { 0,0,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(name_label, { 1,0,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Gender:"), { 0,1,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(gender_label, { 1,1,1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Race"), { 0,2,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(race_label, { 1,2,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(sfg::Label::Create("Occupation"), { 0,3,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(occupation_label, { 1,3,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(sfg::Label::Create("CP available"), { 0,4,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(cp_label, { 1,4,1,1 }, sfg::Table::FILL, sfg::Table::FILL);

	auto attribute_frame = sfg::Frame::Create("Attributes");
	m_attribute_table = Attribute_view::create(nullptr, "");
	attribute_frame->Add(m_attribute_table->widget());
	table->Attach(attribute_frame, { 0,5,2,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	
	auto talent_frame = sfg::Frame::Create("Talents");
	m_talent_table = sfg::Table::Create();
	m_talent_table->SetColumnSpacings(20.0f);
	talent_frame->Add(m_talent_table);
	table->Attach(talent_frame, { 0,6,2,1 }, sfg::Table::FILL, sfg::Table::FILL);
	
	auto disad_frame = sfg::Frame::Create("Disadvantages");
	m_disad_table = sfg::Table::Create();
	m_disad_table->SetColumnSpacings(20.0f);
	disad_frame->Add(m_disad_table);
	table->Attach(disad_frame, { 0,7,2,1 }, sfg::Table::FILL, sfg::Table::FILL);

	auto advantage_frame = sfg::Frame::Create("Advantages");
	m_advantage_table = sfg::Table::Create();
	m_advantage_table->SetColumnSpacings(20.0f);
	advantage_frame->Add(m_advantage_table);
	table->Attach(advantage_frame, { 0,8,2,1 }, sfg::Table::FILL, sfg::Table::FILL);

	m_widget = table;
}

Character_view::Ptr Character_view::create()
{
	return std::make_shared<Character_view>();
}

sfg::Widget::Ptr Character_view::widget()
{
	return m_widget;
}

void Character_view::commit(Character_data& character) {}

int calculate_advantage_cp(const std::vector<Advantage>& advantages)
{
	return std::accumulate(std::cbegin(advantages), std::cend(advantages), 0, [](int so_far, const Advantage& adv) {return so_far + adv.m_cost; });
}

void add_advantages(sfg::Table::Ptr table, const std::vector<Advantage>& advantages)
{
	sf::Uint32 row{ 0 };
	table->RemoveAll();
	for (auto& adv : advantages)
	{
		table->Attach(sfg::Label::Create(adv.m_name), { 0,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		if (adv.m_self_control)
		{
			table->Attach(sfg::Label::Create(std::format("{}%", adv.m_self_control.value())), { 1,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		}
		table->Attach(sfg::Label::Create(std::format("[{}]", adv.m_cost)), { 2,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		++row;
	}
	table->Attach(sfg::Label::Create(std::format("total CP: {}", calculate_advantage_cp(advantages))), { 0,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
}

void add_talents(sfg::Table::Ptr table, const std::span<Talent> talents)
{
	table->RemoveAll();
	for (sf::Uint32 row{ 0 }; auto & talent : talents)
	{
		table->Attach(sfg::Label::Create(talent.m_name), { 0,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		table->Attach(sfg::Label::Create(std::format("{:.2f}", talent.m_value / 100.0f)), { 1,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		auto cost = talent.m_value * talent.m_cost_per_level / 100;
		table->Attach(sfg::Label::Create(std::format("[{}]", cost)), { 2,row,1,1 }, sfg::Table::FILL, sfg::Table::FILL);
		++row;
	}
}

int calculate_talent_cp(const std::vector<Talent>& talents)
{
	return std::accumulate(std::cbegin(talents), std::cend(talents), 0, [](int so_far, const Talent& talent) {return so_far + (talent.m_value * talent.m_cost_per_level / 100); });
}

int calculate_attrib_cp(const std::vector<attributes::Transaction> transactions)
{
	return std::accumulate(std::cbegin(transactions), std::cend(transactions), 0, [](int so_far, const attributes::Transaction& transaction) {return so_far + transaction.points_spent; });
}

int count_cp(const Character_data& character)
{
	return calculate_talent_cp(character.m_talents) +
	       calculate_attrib_cp(character.m_attribute_transactions) +
	       calculate_advantage_cp(character.m_advantages) +
	       calculate_advantage_cp(character.m_disadvantages);
}

int count_cp(const Template& character_template)
{
	auto res = std::accumulate(std::cbegin(character_template.m_modifiers), std::cend(character_template.m_modifiers), 0, [](int so_far, const Attrib_modifier& mod) {return so_far + (attributes::points_per_unit(mod.m_attribute) * mod.m_amount / 100); });
	for (auto x = 0;auto& mod : character_template.m_modifiers)
	{
		auto c_u = attributes::points_per_unit(mod.m_attribute);
		auto c = mod.m_amount * c_u / 100;
		x += c;
	}
	res += calculate_talent_cp(character_template.m_talents);
	res += calculate_advantage_cp(character_template.m_advantages);
	res += calculate_advantage_cp(character_template.m_disadvantages);

	return res;
}

template<typename T>
std::vector<T> merge(const std::vector<T>& first, const std::vector<T>& second)
{
	std::vector<T> res{};
	res.reserve(first.size() + second.size());
	std::ranges::copy(first, std::back_inserter(res));
	std::ranges::copy(second, std::back_inserter(res));
	return res;
}

void Character_view::show(const Character_data& base, const Character_data& delta) const
{
	auto name_label = find_widget("name_label");
	name_label->SetText(delta.m_name);
	auto gender_label = find_widget("gender_label");
	gender_label->SetText(gender_to_string(base.m_gender));
	auto race_label = find_widget("race_label");
	race_label->SetText(delta.m_race);
	auto cp_label = find_widget("cp_label");
	cp_label->SetText(std::to_string(250 - count_cp(base) - count_cp(delta)));

	auto transactions = merge(base.m_attribute_transactions, delta.m_attribute_transactions);
	m_attribute_table->show(transactions);

	auto talents = merge(base.m_talents, delta.m_talents);
	add_talents(m_talent_table, talents);

	auto disadvantages = merge(base.m_disadvantages, delta.m_disadvantages);
	add_advantages(m_disad_table, disadvantages);

	auto advantages = merge(base.m_advantages, delta.m_advantages);
	add_advantages(m_advantage_table, advantages);
}

State_character_generator::State_character_generator(Shared_context* context) : State{ context }
{
	m_db = db::DB_connection::create("assets/database/gamedat.db");
	auto stmt = m_db->prepare("select charpoints from gamesettings");
	stmt.execute_row();
	auto row = stmt.fetch_row();
	m_starting_cp = db::as_int(row["charpoints"]);
}

void Main_view::on_next()
{
	commit(m_delta);
	commit_delta_to_base();
	switch (m_phase)
	{
	case Phase::Name:
		m_phase = Phase::Race;
		break;
	case Phase::Race:
		m_phase = Phase::Occupation;
		break;
	case Phase::Occupation:
		break;
	}
	set_phase(m_phase);
}

void Main_view::commit_delta_to_base()
{
	m_character_so_far = m_delta;
	m_delta.m_disadvantages.clear();
	m_delta.m_attribute_transactions.clear();
	m_delta.m_advantages.clear();
	m_delta.m_talents.clear();
}

void State_character_generator::on_create()
{
	m_context->m_wind->get_renderwindow()->resetGLStates();
	m_gui_window = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
	m_gui_window->SetRequisition(sf::Vector2f(m_context->m_wind->get_renderwindow()->getSize()));
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	auto scrolled_window = sfg::ScrolledWindow::Create();
	scrolled_window->SetScrollbarPolicy(sfg::ScrolledWindow::VERTICAL_AUTOMATIC);
	scrolled_window->SetRequisition(0.9f * sf::Vector2f{ m_context->m_wind->get_renderwindow()->getSize() });

	m_root_widget = m_view.widget();
	m_view.set_db(m_db);
	m_view.set_phase(Phase::Name);

	scrolled_window->AddWithViewport(box);
	box->Pack(m_root_widget);
	m_gui_window->Add(scrolled_window);
	m_desktop.Add(m_gui_window);
}

void State_character_generator::on_destroy()
{

}

void State_character_generator::activate() 
{
	m_root_widget->Show(true);
}

void State_character_generator::deactivate() 
{
	m_root_widget->Show(false);
}

void State_character_generator::update(const sf::Time& time) 
{
	m_desktop.Update(time.asSeconds());
}

void State_character_generator::handle_sfml_event(sf::Event& e) 
{
	m_desktop.HandleEvent(e);
}

void State_character_generator::draw() 
{
	m_context->m_wind->get_renderwindow()->clear();
	m_sfgui.Display(*m_context->m_wind->get_renderwindow());
}