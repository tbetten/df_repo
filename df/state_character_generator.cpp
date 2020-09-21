#include "state_character_generator.h"
#include "shared_context.h"
#include "window.h"
#include "statemanager.h"
//#include "db.h"
#include "db_queries.h"
#include "ecs.h"
#include "components.h"
#include "damage_table.h"
#include "systems.h"
#include "character.h"
#include "drawable.h"
#include "position.h"
#include "attributes.h"
#include "attribute_system.h"
#include "entity_loader.h"
#include "facing_payload.h"
#include "tileset.h"
#include "random_generator.h"
#include "utils.h"

#include <iostream>
#include <string_view>
#include <tuple>
#include <numeric>
#include <charconv>
#include <cassert>

using rect_32 = sf::Rect<sf::Uint32>;

State_character_generator::State_character_generator (Shared_context* context) : State { context }, m_cp { 0 }, m_db { "assets/database/gamedat.db" }, m_cp_fractions (4, 0)
{
	m_cp = DB_queries::query_charpoints ();
	auto talents = DB_queries::get_racial_talents ();
	for (auto [race, name, description, max_level, default_level, cost_per_level] : talents)
	{
		auto skills = DB_queries::get_skills_by_talent (name);
		m_racial_talents.emplace_back (Racial_talent { name, description, string_to_race (race), max_level, default_level, cost_per_level, skills });
	}
	auto racial_advantages = DB_queries::get_racial_advantages ();
	for (auto [advantage, description, race, default_level, max_level, cost_per_level, uses_selfcontrol, selfcontrol] : racial_advantages)
	{
		bool sc_flag = uses_selfcontrol == 0 ? false : true;
		m_racial_advantages.emplace_back (Advantage { advantage, description, string_to_race (race), default_level, max_level, cost_per_level, sc_flag, selfcontrol });
	}
}

void State_character_generator::on_create ()
{
	read_attributes ();
	read_attrib_modifiers ();
	m_context->m_wind->get_renderwindow ()->resetGLStates ();
	m_gui_window = sfg::Window::Create (sfg::Window::Style::NO_STYLE);
	m_gui_window->SetRequisition (sf::Vector2f (m_context->m_wind->get_renderwindow ()->getSize ()));

	auto box = sfg::Box::Create (sfg::Box::Orientation::VERTICAL);
	auto character_table = sfg::Table::Create ();
	m_name_entry = sfg::Entry::Create ();
	m_race = sfg::Label::Create ("Human");
	m_male = sfg::RadioButton::Create ("Male");
	m_female = sfg::RadioButton::Create ("Female", m_male->GetGroup ());
	m_male->SetActive (true);
	character_table->Attach (sfg::Label::Create ("Name"), rect_32 { 0, 0, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach (m_name_entry, rect_32 { 1, 0, 2, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach (sfg::Label::Create ("Race"), rect_32 { 0, 1, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach (m_race, rect_32 { 1, 1, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach (sfg::Label::Create ("Gender"), rect_32 { 0, 2, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach (m_male, rect_32 { 1, 2, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach (m_female, rect_32 { 2, 2, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->SetColumnSpacings (20.0f);
	character_table->SetRowSpacings (10.0f);

	auto races_table = read_races ();

	auto attrib_table = create_attribute_table ();
	m_attrib_box = sfg::Box::Create ();
	m_attrib_box->Pack (attrib_table);

	auto notebook = sfg::Notebook::Create ();
	notebook->SetId ("notebook");
	notebook->AppendPage (character_table, sfg::Label::Create ("Character"));
	notebook->AppendPage (races_table, sfg::Label::Create ("Race"));
	notebook->AppendPage (m_attrib_box, sfg::Label::Create ("Attributes"));

	//	auto vbox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	//	auto hbox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	//	hbox->SetSpacing(50.0f);
	//	vbox->SetSpacing(20.0f);
	//	hbox->Pack(notebook, false);
	auto party_box = sfg::Box::Create (sfg::Box::Orientation::VERTICAL);
	party_box->SetId ("party_box");
	//	hbox->Pack(party_box);
	//	vbox->Pack(hbox, false);
	auto keep_button = sfg::Button::Create ("Keep");
	keep_button->GetSignal (sfg::Button::OnLeftClick).Connect ([this] () {on_keep (); });
	//keep_button->GetSignal(sfg::Button::OnLeftClick).Connect([]() {std::cout << "keep!\n"; });
	auto finish_button = sfg::Button::Create ("Finish");
	finish_button->GetSignal (sfg::Button::OnLeftClick).Connect ([this] () {on_finish (); });
	//	auto button_box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	//	button_box->Pack(keep_button, false, false);
	//	button_box->Pack(finish_button, false, false);
	//	auto lower_box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	//	lower_box->Pack(button_box);
	//	auto right_lower = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	//	lower_box->Pack(right_lower);
	auto delete_button = sfg::Button::Create ("delete");
	delete_button->GetSignal (sfg::Button::OnLeftClick).Connect ([this] () {remove_party_members (); });
	//	right_lower->Pack(delete_button, false);
	//	vbox->Pack(lower_box, false, false);
	auto table = sfg::Table::Create ();
	table->SetColumnSpacing (1, 50.0f);
	table->SetRowSpacings (20.0f);
	table->Attach (notebook, sf::Rect<sf::Uint32>{0, 0, 2, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (keep_button, sf::Rect<sf::Uint32>{0, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (finish_button, sf::Rect<sf::Uint32>{1, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (party_box, sf::Rect<sf::Uint32>{2, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (delete_button, sf::Rect<sf::Uint32>{2, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);


	m_root_widget = sfg::Box::Create (sfg::Box::Orientation::HORIZONTAL);

	m_root_widget->Pack (table, false);

	m_gui_window->Add (m_root_widget);
	m_desktop.Add (m_gui_window);


}

void State_character_generator::read_attributes ()
{
	auto sql = "select id, base from attribute";
	auto stmt = m_db.prepare (sql);
	auto attribs = stmt.fetch_table ();
	for (auto attrib : attribs)
	{
		auto name = std::get<std::string> (attrib ["id"]);
		auto base = std::get<int> (attrib ["base"]);
		m_attrib_values.emplace_back (Attrib_value { name, base, 0 });
	}
}

std::string State_character_generator::make_cost_label (int level)
{
	return "[" + std::to_string (level * m_cost_per_level) + "]";
}

int State_character_generator::find_chooser (const std::string& id)
{
	auto itr = std::find_if (std::cbegin (m_choosers), std::cend (m_choosers), [id] (const Chooser& c) {return id == c.id; });
	assert (itr != std::cend (m_choosers));
	return std::distance (std::cbegin (m_choosers), itr);
}

void State_character_generator::on_chooser_changed (const std::string& id, CP_fractions fraction)
{
	auto chooser_index = find_chooser (id);
	auto& chooser = m_choosers [chooser_index];
	auto new_value = static_cast<int>(chooser.widget->GetValue ());
	auto delta = (new_value - chooser.value) * chooser.cost_per_step;
	chooser.value = new_value;
	m_cp_fractions [static_cast<int>(fraction)] -= delta;
	m_cp -= delta;
	update_cp_label ();
	auto cost_label = find_widget<sfg::Label> ("cost_label");
	cost_label->SetText (make_cost_label (new_value));
}

sfg::Box::Ptr State_character_generator::read_races ()
{
	const std::string sql = "select name, is_default, template_cost from race";
	auto stmt = m_db.prepare (sql);
	auto data = stmt.fetch_table ();

	auto table = sfg::Table::Create ();
	table->SetRowSpacings (10.0f);
	table->SetColumnSpacings (20.0f);
	std::shared_ptr<sfg::RadioButtonGroup> group = nullptr;

	sf::Uint32 index { 0 };
	for (auto row : data)
	{
		unsigned int template_cost = std::get<int> (row ["template_cost"]);
		auto name = std::get<std::string> (row ["name"]);
		bool is_default = std::get<std::string> (row ["is_default"]) == "Y" ? true : false;
		auto rb = sfg::RadioButton::Create (name + " [" + std::to_string (template_cost) + "]");
		if (group == nullptr) group = rb->GetGroup ();
		m_race_buttons [name] = rb;
		rb->SetGroup (group);
		if (is_default) rb->SetActive (true);
		rb->GetSignal (sfg::RadioButton::OnToggle).Connect ([this, name] () {on_race_toggle (name); });
		table->Attach (rb, rect_32 { index % 2, index / 2, 1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
		++index;
	}
	auto inner = sfg::Box::Create (sfg::Box::Orientation::VERTICAL);
	inner->SetId ("inner_box");
	inner->SetSpacing (20.0f);
	m_cp_label = sfg::Label::Create ("");
	update_cp_label ();
	inner->Pack (m_cp_label);
	inner->Pack (table, false);
	inner->Pack (sfg::Separator::Create (), false);
	auto talent_table = sfg::Table::Create ();
	talent_table->SetRequisition (sf::Vector2f { 300.0f, 0.0f });
	talent_table->SetId ("racial_talent_table");
	talent_table->SetRowSpacings (30.0f);
	talent_table->SetColumnSpacings (20.0f);
	auto talent_heading = sfg::Label::Create ("Racial Talent");
	talent_heading->SetId ("talent_heading");
	talent_heading->Show (false);
	talent_table->Attach (talent_heading, sf::Rect<sf::Uint32>{0, 0, 4, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	auto talent_desc = sfg::Label::Create ("A talent gives a bonus equal to its level to all listed skills. The racial talent level can only be set at character creation.");
	talent_desc->SetId ("talent_desc");
	talent_desc->SetLineWrap (true);
	talent_desc->Show (false);
	talent_table->Attach (talent_desc, sf::Rect<sf::Uint32>{0, 1, 4, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	auto talent_name = sfg::Label::Create ("");
	talent_name->SetId ("talent_name");
	talent_table->Attach (talent_name, sf::Rect<sf::Uint32>{0, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	auto talent_description = sfg::Label::Create ("");
	talent_description->SetId ("talent_description");
	talent_description->SetLineWrap (true);
	talent_table->Attach (talent_description, sf::Rect<sf::Uint32>{0, 2, 4, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	auto cost_label = sfg::Label::Create ("cost_label");
	Chooser racial_talent_chooser;
	racial_talent_chooser.widget = sfg::SpinButton::Create (0, 0, 0);
	std::string id = "racial_talent_chooser";
	racial_talent_chooser.widget->SetId (id);
	racial_talent_chooser.widget->SetRequisition (sf::Vector2f (50.0f, 0.0f));
	racial_talent_chooser.widget->GetSignal (sfg::SpinButton::OnValueChanged).Connect ([this, id, fraction = CP_fractions::Racial_talent] () {on_chooser_changed (id, fraction); });
	racial_talent_chooser.widget->Show (false);
	racial_talent_chooser.id = racial_talent_chooser.widget->GetId ();
	racial_talent_chooser.value = 0;
	m_choosers.push_back (racial_talent_chooser);
	//	auto level_chooser = sfg::SpinButton::Create(0, 0, 0);
	//	level_chooser->SetId("level_chooser");
	//	level_chooser->SetRequisition(sf::Vector2f(50.0f, 0.0f));
	//	level_chooser->GetSignal(sfg::SpinButton::OnValueChanged).Connect([this, level_chooser, cost_label]() 
	//		{
	//			cost_label->SetText(make_cost_label(static_cast<int>(level_chooser->GetValue())));
	//		});
	//	level_chooser->Show(false);
	talent_table->Attach (racial_talent_chooser.widget, sf::Rect<sf::Uint32>{1, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::EXPAND);

	cost_label->SetId ("cost_label");
	talent_table->Attach (cost_label, sf::Rect<sf::Uint32>{2, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	inner->Pack (talent_table);
	inner->Pack (sfg::Separator::Create ());
	auto advantages = sfg::Table::Create ();
	advantages->SetId ("advantage_table");
	advantages->SetColumnSpacings (20.0f);
	inner->Pack (advantages);
	//inner->Pack(create_racial_talent());
	auto outer = sfg::Box::Create (sfg::Box::Orientation::HORIZONTAL);
	outer->SetSpacing (20.0f);
	outer->Pack (inner, false);
	return outer;
}

void State_character_generator::update_cp_label ()
{
	m_cp_label->SetText ("Available Character Points: " + std::to_string (m_cp));
}

void State_character_generator::create_racial_talent (const std::string& race)
{
	auto talent_heading = find_widget ("talent_heading");
	auto talent_desc = find_widget ("talent_desc");
	auto talent_name = find_widget ("talent_name");
	auto talent_description = find_widget ("talent_description");
	//auto level_chooser = std::dynamic_pointer_cast<sfg::SpinButton>(sfg::Widget::GetWidgetById("level_chooser"));
	auto cost_label = find_widget ("cost_label");
	auto& racial_talent_chooser = m_choosers [find_chooser ("racial_talent_chooser")];

	auto talent_itr = std::find_if (std::cbegin (m_racial_talents), std::cend (m_racial_talents), [race] (const Racial_talent& talent) {return talent.race == string_to_race (race); });

	if (talent_itr == std::cend (m_racial_talents))
	{
		talent_heading->Show (false);
		talent_desc->Show (false);
		talent_name->Show (false);
		talent_description->Show (false);
		//level_chooser->Show(false);
		racial_talent_chooser.widget->Show (false);
		racial_talent_chooser.value = 0;
		racial_talent_chooser.cost_per_step = 0;
		cost_label->Show (false);
		return;
	}
	auto talent = *talent_itr;
	m_cost_per_level = talent.cost_per_level;
	talent_heading->Show (true);
	talent_desc->Show (true);
	talent_name->SetText (talent.name);
	talent_name->Show (true);
	std::string description { talent.description };
	description += "\nThis talent increases the following skills: ";
	description += talent.skills.front ();
	description = std::accumulate (std::next (std::cbegin (talent.skills)), std::cend (talent.skills), description, [] (std::string so_far, std::string next) {return so_far + ", " + next; });
	talent_description->SetText (description);
	talent_description->Show (true);
	racial_talent_chooser.cost_per_step = talent.cost_per_level;
	racial_talent_chooser.value = 0;
	racial_talent_chooser.widget->SetRange (static_cast<float>(talent.default_level), static_cast<float>(talent.max_levels));
	racial_talent_chooser.widget->SetStep (1.0f);
	racial_talent_chooser.widget->Show (true);

	racial_talent_chooser.widget->SetValue (static_cast<int>(talent.default_level));
	cost_label->SetText (make_cost_label (talent.default_level));
	//m_cp -= default_level * m_cost_per_level;
	cost_label->Show (true);

}

void State_character_generator::on_race_toggle (const std::string& name)
{
	if (m_race_buttons [name]->IsActive ())
	{
		m_choosers [find_chooser ("racial_talent_chooser")].reset ();
		m_race->SetText (name);
		auto race = string_to_race (name);
		m_cp += std::accumulate (std::cbegin (m_attribute_transactions), std::cend (m_attribute_transactions), 0, [] (int points_so_far, const attributes::Transaction& transaction) {return points_so_far + transaction.points_spent; });
		m_cp -= m_cp_fractions [static_cast<int>(CP_fractions::Racial_talent)];
		m_cp_fractions [static_cast<int>(CP_fractions::Racial_talent)] = 0;
		update_cp_label ();
		m_attribute_transactions.erase (std::remove_if (std::begin (m_attribute_transactions), std::end (m_attribute_transactions), [] (const attributes::Transaction& transaction) {return transaction.template_type == attributes::Template_type::Race; }), std::end (m_attribute_transactions));

		for (auto current = std::find_if (std::cbegin (m_modifiers), std::cend (m_modifiers), [race] (const Modifier& modifier) {return modifier.race == race; }); current != std::cend (m_modifiers); current = std::find_if (current, std::cend (m_modifiers), [race] (const Modifier& modifier) { return modifier.race == race; }))
		{
			auto cost = attributes::points_per_unit (current->attribute) * current->amount;
			m_cp -= cost;
			//m_attribute_transactions.emplace_back (attributes::Transaction { current->attribute, attributes::Transaction_type::Buy, attributes::Template_type::Race, name, current->amount * 100, attributes::points_per_unit (current->attribute) * current->amount });
			m_attribute_transactions.emplace_back (current->attribute, attributes::Transaction_type::Buy, attributes::Template_type::Race, name, current->amount * 100, attributes::points_per_unit (current->attribute) * current->amount, std::nullopt);
			auto raises = attributes::raises_base (current->attribute);
			for (auto [attribute, amount] : raises)
			{
				//m_attribute_transactions.emplace_back (attributes::Transaction { attribute, attributes::Transaction_type::Raise_base, attributes::Template_type::Race, name, current->amount * amount, 0 });
				m_attribute_transactions.emplace_back (attribute, attributes::Transaction_type::Raise_base, attributes::Template_type::Race, name, current->amount * amount, 0, std::nullopt);
			}
			current = std::next (current);
		}
		//auto cp_label = std::dynamic_pointer_cast<sfg::Label> (sfg::Widget::GetWidgetById("cp_label"));
		//cp_label->SetText("Available Character Points: " + std::to_string(m_cp));
		update_cp_label ();
		m_attrib_box->RemoveAll ();
		m_attrib_box->Pack (create_attribute_table ());

		create_racial_talent (name);
		auto list = find_widget<sfg::Table> ("advantage_table");
		list->RemoveAll ();
		auto end = std::remove_if (std::begin (m_racial_advantages), std::end (m_racial_advantages), [race] (Advantage& advantage) {return (!std::holds_alternative<Race> (advantage.template_name)) || (std::get<Race> (advantage.template_name) != race); });
		sf::Uint32 i { 0 };
		std::for_each (std::begin (m_racial_advantages), end, [list, &i] (Advantage& adv)
			{
				auto desc = sfg::Label::Create (adv.description);
				auto cost_label = sfg::Label::Create ("[" + std::to_string (adv.default_level * adv.cost_per_level) + "]");
				//desc->SetLineWrap(true);
				list->Attach (sfg::Label::Create (adv.name), sf::Rect<sf::Uint32>{0, i, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
				list->Attach (sfg::Label::Create (std::to_string (adv.default_level)), sf::Rect<sf::Uint32>{1, i, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
				list->Attach (cost_label, sf::Rect<sf::Uint32>{2, i, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
				list->Attach (desc, sf::Rect<sf::Uint32>{3, i, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
				++i;
			});
		for (auto advantage : m_racial_advantages)
		{
			if (std::holds_alternative<Race> (advantage.template_name) && std::get<Race> (advantage.template_name) == race)
			{
				std::cout << advantage.name << "\n";
			}
		}
	}
}

void State_character_generator::on_keep ()
{
	Character_data character;
	std::string race_str = "";
	std::string gender_str = "";
	character.name = m_name_entry->GetText ();
	if (m_male->IsActive ())
	{
		gender_str = "Male";
		character.gender = Gender::Male;
	}
	else
	{
		gender_str = "Female";
		character.gender = Gender::Female;

	}

	for (auto [race, button] : m_race_buttons)
	{
		if (button->IsActive ())
		{
			race_str = race;
			character.race = string_to_race (race);
		}
	}
	auto [tile_index, sheet] = DB_queries::query_icon_key (race_str, gender_str);
	character.tile_sheet = sheet;
	character.tile_index = tile_index;
	if (character.gender == Gender::Male)
	{
		if (character.race == Race::Human && Random_generator::generate_discrete ({ 70, 30 }) == 0)
		{
			character.beard_index = Random_generator::generate_uniform (0, 10);
		}
		if (character.race == Race::Dwarf)
		{
			character.beard_index = Random_generator::generate_discrete ({ 15, 15, 15, 15, 15, 2, 4, 4, 4, 4, 4 });
		}
	}

	//auto last = std::remove_if(m_attrib_values.begin(), m_attrib_values.end(), [](const Attrib_value& attrib_val) {return attrib_val.modifier == 0; });
	//std::for_each(m_attrib_values.begin(), last, [&character](const Attrib_value attrib_val) {character.attrib_modifiers.push_back(attrib_val); });
	std::copy (std::begin (m_attribute_transactions), std::end (m_attribute_transactions), std::back_inserter (character.transactions));
	m_attribute_transactions.clear ();

	auto party_box = std::dynamic_pointer_cast<sfg::Box> (sfg::Widget::GetWidgetById ("party_box"));

	auto button = sfg::CheckButton::Create (character.name + ", " + gender_str + " " + race_to_string (character.race));
	button->SetId (character.name);

	//std::string s{character.name + ", " + gender_str + " " + race_to_string(character.race)};
	party_box->PackEnd (button);

	m_party.push_back (character);
}

void State_character_generator::remove_party_members ()
{
	auto box = std::dynamic_pointer_cast<sfg::Box>(sfg::Widget::GetWidgetById ("party_box"));
	auto buttons = box->GetChildren ();
	std::vector<std::string> names;
	std::transform (std::begin (buttons), std::end (buttons), std::back_inserter (names), [] (sfg::Widget::Ptr widget)
		{
			auto button = std::dynamic_pointer_cast<sfg::CheckButton>(widget);
			if (button->IsActive ()) return button->GetId ();
		});

}

void State_character_generator::on_finish ()
{
	for (auto member : m_party)
	{
		auto em = m_context->m_entity_manager;
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Character));
		b.set (static_cast<int>(ecs::Component_type::Drawable));
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set (static_cast<int>(ecs::Component_type::Facing));
		b.set (static_cast<int>(ecs::Component_type::Attributes));
		b.set (static_cast<int>(ecs::Component_type::Container));
		auto entity = em->add_entity (b);
		m_context->m_party.push_back (entity);

		auto character = em->get_data<ecs::Component<Character>> (ecs::Component_type::Character, entity);
		character->name = member.name;
		character->gender = member.gender;
		character->race = member.race;
		character->user_controlled = true;
		character->character_points = m_cp;

		//auto attributes = em->get_data<ecs::Component<Attributes>>(ecs::Component_type::Attributes, entity);
		auto attrib_system = m_context->m_system_manager->get_system<systems::Attribute_system> (ecs::System_type::Attributes);
		std::for_each (std::cbegin (member.transactions), std::cend (member.transactions), [entity, attrib_system] (const attributes::Transaction& transaction) {attrib_system->add_transaction (transaction, entity); });
		//for (auto attrib : member.attrib_modifiers)
		//{
		//	buy_attrib(attrib, entity);
		//}

		init_drawable (m_context->m_entity_manager, entity, member.tile_sheet, m_context->m_cache, member.tile_index);
		if (member.beard_index >= 0)
		{
			fill_icon_part (m_context->m_entity_manager, m_context->m_cache, "beards", member.beard_index, "beard", entity);
		}
	}
	el::Entity_loader el { m_context };
	el.load_map ("test");
	m_context->m_current_map = "test";
	int i { 0 };
	for (auto e : m_context->m_party)
	{
		el.set_position (e, sf::Vector2i { 4, 2 + i }, Position::Layer::Creature, "test");
		++i;
	}
	m_context->m_state_manager->switch_to (Game_state::Game);
	m_done = true;
	/*	el::Entity_loader el{ m_context };
		el.load_map("test");
		m_context->m_current_map = "test";
		m_context->m_state_manager->switch_to(Game_state::Game);
		m_done = true;*/
}


void State_character_generator::buy_attrib (Attrib_value a, ecs::Entity_id entity)
{
	auto attribute_system = m_context->m_system_manager->get_system<systems::Attribute_system> (ecs::System_type::Attributes);
	auto name = a.name;
	auto modifier = a.modifier;
	//attribute_system->buy_units(attributes::string_to_attrib(name), entity, modifier * 100);
//	attribute_system->buy_attribute(string_to_attrib[name], entity, modifier);
}

std::string format_attribute_value (int points_spent, int value, bool with_cost)
{
	std::string spent {};
	if (with_cost)
	{
		spent = "[" + std::to_string (points_spent) + "]";
		spent.insert (0, 5 - spent.size (), ' ');
	}
	auto v = static_cast<double>(value) / 100.0;
	std::string buffer { "xxxxx" };
	auto res = std::to_chars (buffer.data (), buffer.data () + buffer.size (), v, std::chars_format::fixed, 2);
	auto i = buffer.find ("x");
	if (i != std::string::npos)
	{
		auto num = buffer.size () - i;
		buffer.erase (i, num);
		buffer.insert (0, num, ' ');
	}
	return buffer + " " + spent;
}

void attach_attribute (sfg::Table::Ptr table, sf::Uint32 column, sf::Uint32 row, attributes::Attrib attribute, std::span<attributes::Transaction> transactions)
{
	auto points_spent = attributes::get_spent_points (transactions, attribute);
	auto val = attributes::get_total_value (transactions, attribute);
	auto label = format_attribute_value (points_spent, val, attributes::format_with_cost[attribute]);
	table->Attach (sfg::Label::Create (attributes::attrib_to_string (attribute)), rect_32 { column, row, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (sfg::Label::Create (label), rect_32 { column + 1, row, 1, 1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
}

void attach_attributes (sfg::Table::Ptr table, sf::Uint32 column, std::span<attributes::Attrib> attributes, std::span<attributes::Transaction> transactions)
{
	sf::Uint32 index { 0 };
	for (auto attribute : attributes)
	{
		attach_attribute (table, column, index, attribute, transactions);
		++index;
	}
}

sfg::Table::Ptr State_character_generator::create_attribute_table ()
{
	auto table = sfg::Table::Create ();
	table->SetColumnSpacings (20.0f);
	table->SetRowSpacings (10.0f);
	sf::Uint32 index { 0 };
	attach_attributes (table, 0, attributes::primary_attributes, m_attribute_transactions);
	attach_attributes (table, 2, attributes::secundary_attributes, m_attribute_transactions);
	attach_attributes (table, 4, attributes::tertiary_attributes, m_attribute_transactions);
	attach_attribute (table, 4, 3, attributes::Attrib::BL, m_attribute_transactions);
	attach_attribute (table, 0, 6, attributes::Attrib::ST_lift, m_attribute_transactions);
	attach_attribute (table, 2, 6, attributes::Attrib::ST_strike, m_attribute_transactions);
	auto dodge = attributes::get_total_value (m_attribute_transactions, attributes::Attrib::Dodge);
	constexpr damage::Damage_table t {};
	auto strike_strength = (attributes::get_total_value (m_attribute_transactions, attributes::Attrib::ST) + attributes::get_total_value (m_attribute_transactions, attributes::Attrib::ST_strike)) / 100;
	auto& [strength, thrust_dice, swing_dice, thrust_mean, thrust_stddev, swing_mean, swing_stddev] = t.table [strike_strength];
	table->Attach (sfg::Label::Create ("Damage (thrust)"), sf::Rect<sf::Uint32>{0, 7, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (sfg::Label::Create (format_attribute_value (0, thrust_mean * 100, false)), sf::Rect<sf::Uint32>{1, 7, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (sfg::Label::Create ("Damage (swing)"), sf::Rect<sf::Uint32>{2, 7, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (sfg::Label::Create (format_attribute_value (0, swing_mean * 100, false)), sf::Rect<sf::Uint32>{3, 7, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (sfg::Label::Create ("Dodge"), sf::Rect<sf::Uint32>{0, 8, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach (sfg::Label::Create (format_attribute_value (0, dodge, false)), sf::Rect<sf::Uint32>{1, 8, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	return table;
}

void State_character_generator::apply_attrib_modifier (const std::string& name, int modifier)
{
	if (name == "ST")
	{
		auto hp_itr = std::find_if (std::begin (m_attrib_values), std::end (m_attrib_values), [] (const Attrib_value& attrib) {return attrib.name == "HP"; });
		hp_itr->base += modifier;
	}
	if (name == "IQ")
	{
		auto will_itr = std::find_if (std::begin (m_attrib_values), std::end (m_attrib_values), [] (const Attrib_value& attrib) {return attrib.name == "Will"; });
		will_itr->base += modifier;
		auto per_itr = std::find_if (std::begin (m_attrib_values), std::end (m_attrib_values), [] (const Attrib_value& attrib) {return attrib.name == "Per"; });
		per_itr->base += modifier;
	}
	if (name == "HT")
	{
		auto fp_itr = std::find_if (std::begin (m_attrib_values), std::end (m_attrib_values), [] (const Attrib_value& attrib) {return attrib.name == "FP"; });
		fp_itr->base += modifier;
	}
}

void State_character_generator::read_attrib_modifiers ()
{
	auto stmt = m_db.prepare ("select race, attribute, modifier from race_attribute");
	auto modifiers = stmt.fetch_table ();
	for (auto modifier : modifiers)
	{
		auto race = string_to_race (std::get<std::string> (modifier ["race"]));
		auto amount = std::get<int> (modifier ["modifier"]);
		auto attribute = attributes::string_to_attrib (std::get<std::string> (modifier ["attribute"]));
		m_modifiers.emplace_back (Modifier { race, attribute, amount });
	}

	/*	for (auto& attrib : m_attrib_values)
		{
			attrib.modifier = 0;
		}
		const std::string sql = "select attribute, modifier from race_attribute where race = ?";
		auto stmt = m_db.prepare(sql);
		stmt.bind(1, race);
		auto modifiers = stmt.fetch_table();
		for (auto modifier : modifiers)
		{
			auto attribute = std::get<std::string>(modifier["attribute"]);
			auto mod = std::get<int>(modifier["modifier"]);
			auto itr = std::find_if(m_attrib_values.begin(), m_attrib_values.end(), [attribute](Attrib_value attrib) {return attrib.name == attribute; });
			if (itr != m_attrib_values.end()) itr->modifier = mod;*/
			/*		if (m_attrib_values.find(attribute) != m_attrib_values.end())
					{
						m_attrib_values[attribute].modifier = mod;
					}
				}*/
}

void State_character_generator::on_destroy ()
{

}

void State_character_generator::update (const sf::Time& time)
{
	m_desktop.Update (time.asSeconds ());
}

void State_character_generator::handle_sfml_event (sf::Event& e)
{
	m_desktop.HandleEvent (e);
}

void State_character_generator::activate ()
{
	m_root_widget->Show (true);
}

void State_character_generator::deactivate ()
{
	m_root_widget->Show (false);
}

void State_character_generator::draw ()
{
	m_context->m_wind->get_renderwindow ()->clear ();
	m_sfgui.Display (*m_context->m_wind->get_renderwindow ());
}

void State_character_generator::to_game ()
{
	m_context->m_state_manager->switch_to (Game_state::Game);
}