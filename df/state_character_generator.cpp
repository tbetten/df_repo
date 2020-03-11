#include "state_character_generator.h"
#include "shared_context.h"
#include "window.h"
#include "statemanager.h"
//#include "db.h"
#include "ecs.h"
#include "components.h"
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

#include <iostream>
#include <string_view>

using rect_32 = sf::Rect<sf::Uint32>;

State_character_generator::State_character_generator(Shared_context* context) : State{ context }, m_cp{ 0 }, m_db{"assets/database/gamedat.db"}
{
	//m_db = db::db_connection::create("assets/database/gamedat.db");
	auto sql = "select charpoints from gamesettings";
	auto stmt = m_db.prepare(sql);
	auto rs = stmt.fetch_table();
	m_cp = std::get<int>(rs[0]["charpoints"]);
	stmt.reset();
}

void State_character_generator::on_create()
{
	read_attributes();
	m_context->m_wind->get_renderwindow()->resetGLStates();
	m_gui_window = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
	m_gui_window->SetRequisition(sf::Vector2f(m_context->m_wind->get_renderwindow()->getSize()));
	
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	auto character_table = sfg::Table::Create();
	m_name_entry = sfg::Entry::Create();
	m_race = sfg::Label::Create("Human");
	m_male = sfg::RadioButton::Create("Male");
	m_female = sfg::RadioButton::Create("Female", m_male->GetGroup());
	m_male->SetActive(true);
	character_table->Attach(sfg::Label::Create("Name"), rect_32{0, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach(m_name_entry, rect_32{1, 0, 2, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach(sfg::Label::Create("Race"), rect_32{0, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach(m_race, rect_32{1, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach(sfg::Label::Create("Gender"), rect_32{0, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach(m_male, rect_32{1, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->Attach(m_female, rect_32{2, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	character_table->SetColumnSpacings(20.0f);
	character_table->SetRowSpacings(10.0f);

	auto races_table = read_races();

	auto attrib_table = create_attribute_table();
	m_attrib_box = sfg::Box::Create();
	m_attrib_box->Pack(attrib_table);
	
	auto notebook = sfg::Notebook::Create();
	notebook->SetId("notebook");
	notebook->AppendPage(character_table, sfg::Label::Create("Character"));
	notebook->AppendPage(races_table, sfg::Label::Create("Race"));
	notebook->AppendPage(m_attrib_box, sfg::Label::Create("Attributes"));

	auto vbox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	vbox->SetSpacing(20.0f);
	vbox->Pack(notebook, false);
	auto keep_button = sfg::Button::Create("Keep");
	keep_button->GetSignal(sfg::Button::OnLeftClick).Connect([this]() {on_keep(); });
	auto button_box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	button_box->Pack(keep_button, false, false);
	vbox->Pack(button_box, false, false);
	m_root_widget = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);

	m_root_widget->Pack(vbox, false);
	m_gui_window->Add(m_root_widget);
	m_desktop.Add(m_gui_window);
}

void State_character_generator::read_attributes()
{
	auto sql = "select id, base from attribute";
	auto stmt = m_db.prepare(sql);
	auto attribs = stmt.fetch_table();
	for (auto attrib : attribs)
	{
		auto name = std::get<std::string>(attrib["id"]);
		auto base = std::get<int>(attrib["base"]);
		m_attrib_values.emplace_back(Attrib_value{name, base, 0 });
	}
}

sfg::Box::Ptr State_character_generator::read_races()
{
	const std::string sql = "select name, is_default, template_cost from race";
	auto stmt = m_db.prepare(sql);
	auto data = stmt.fetch_table();

	auto table = sfg::Table::Create();
	table->SetRowSpacings(10.0f);
	table->SetColumnSpacings(20.0f);
	std::shared_ptr<sfg::RadioButtonGroup> group = nullptr;

	sf::Uint32 index{ 0 };
	for (auto row : data)
	{
		unsigned int template_cost = std::get<int>(row["template_cost"]);
		auto name = std::get<std::string>(row["name"]);
		bool is_default = std::get<std::string>(row["is_default"]) == "Y" ? true : false;
		std::cout << std::boolalpha << name << "\t" << is_default << "\n";
		auto rb = sfg::RadioButton::Create (name + " [" + std::to_string(template_cost) + "]");
		if (group == nullptr) group = rb->GetGroup();
		m_race_buttons[name] = rb;
		rb->SetGroup(group);
		if (is_default) rb->SetActive(true);
		rb->GetSignal(sfg::RadioButton::OnToggle).Connect([this, name]() {on_race_toggle(name); });
		table->Attach(rb, rect_32{ index % 2, index / 2, 1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
		++index;
	}
	auto inner = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	inner->SetSpacing(20.0f);
	inner->Pack(sfg::Label::Create("Available Character Points: " + std::to_string(m_cp)));
	inner->Pack(table, false);
	inner->Pack(sfg::Separator::Create(), false);
	inner->Pack(sfg::Label::Create("Racial talent"));
	auto outer = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	outer->Pack(inner, false);
	return outer;
}

std::string get_icon_key(db::DB_connection& db, const std::string& race, const std::string& gender)
{
	auto stmt = db.prepare("select icon_key from player_icons where race = ? and gender = ?");
	stmt.bind(1, race);
	stmt.bind(2, gender);
	stmt.execute_row();
	auto row = stmt.fetch_row();
	stmt.reset();
	return std::get<std::string>(row["icon_key"]);
}

void State_character_generator::on_race_toggle(const std::string& name)
{
	if (m_race_buttons[name]->IsActive())
	{
		m_race->SetText(name);
		std::for_each(std::begin(m_attrib_values), std::end(m_attrib_values), [](Attrib_value& attrib) {attrib.base = 10; });
		read_attrib_modifiers(name);
		m_attrib_box->RemoveAll();
		m_attrib_box->Pack(create_attribute_table());
	}
}

void State_character_generator::on_keep()
{
	std::string name = m_name_entry->GetText();
	bool  gender{ true };
	if (m_male->IsActive()) gender = true;
	if (m_female->IsActive()) gender = false;
	std::string gender_string = gender ? "Male" : "Female";
	std::string race;
	for (auto button : m_race_buttons)
	{
		if (button.second->IsActive())
		{
			race = button.first;
			break;
		}
	}
	auto em = m_context->m_entity_manager;
	ecs::Bitmask b;
	b.set(static_cast<int>(ecs::Component_type::Character));
	b.set(static_cast<int>(ecs::Component_type::Drawable));
	b.set(static_cast<int>(ecs::Component_type::Position));
	b.set(static_cast<int>(ecs::Component_type::Facing));
	b.set(static_cast<int>(ecs::Component_type::Attributes));
	b.set(static_cast<int>(ecs::Component_type::Container));
	auto entity = em->add_entity(b);
	m_context->m_party.push_back(entity);
	auto character = em->get_data<ecs::Component<Character>>(ecs::Component_type::Character, entity);
	character->name = name;
	character->male = gender;
	character->race = string_to_race(race);
	character->user_controlled = true;
	character->character_points = m_cp;
	std::cout << name << " is a " << gender_string << " " << race << "\n";

	auto icon_key = get_icon_key(m_db, race, gender_string);
	std::cout << "icon is " << icon_key << "\n";
	init_drawable(m_context->m_entity_manager, entity, icon_key, m_context->m_cache);
	fill_icon_part(m_context->m_entity_manager, m_context->m_cache, "facing_ne", "facing_indicator", entity);
	if (character->male)
	{
		int beard_index = -1;
		if (character->race == Race::Human)
		{
			if (Random_generator::generate_discrete({ 70, 30 }) == 0)
			{
				beard_index = Random_generator::generate_uniform(0, 10);
			}
		}
		if (character->race == Race::Dwarf)
		{
			beard_index = Random_generator::generate_discrete({ 15, 15, 15, 15, 15, 2, 4, 4, 4, 4, 4 });
		}
		if (beard_index > -1)
		{
			std::cout << "beard index: " << beard_index << "\n";
			tileset::Tileset ts{};
			tileset::load_tileset("assets/sprite/player_icon/beard/beards.tsx", ts, m_context);
			auto rect = ts.get_rect(beard_index);
			fill_icon_part(m_context->m_entity_manager, ts.texture, rect, "beard", entity);
		}
	}
	auto last = std::remove_if(m_attrib_values.begin(), m_attrib_values.end(), [](auto attrib_val) {return attrib_val.modifier == 0; });
	std::for_each(m_attrib_values.begin(), last, [this, entity](auto attrib_val) {buy_attrib(attrib_val, entity); });
	el::Entity_loader el{m_context};
	el.load_map("test");
	m_context->m_current_map = "test";
	el.set_position(entity, sf::Vector2i{ 4,6 }, 2, "test");
	m_context->m_state_manager->switch_to(Game_state::Game);
	m_done = true;
}

std::unordered_map<std::string, Attribute> string_to_attrib{ {"ST", Attribute::ST}, {"DX", Attribute::DX}, {"IQ", Attribute::IQ}, {"HT", Attribute::HT},
{"HP", Attribute::HP}, {"Will", Attribute::Will }, {"Per", Attribute::Per}, {"FP", Attribute::FP } };

void State_character_generator::buy_attrib(Attrib_value a, ecs::Entity_id entity)
{
	auto attribute_system = m_context->m_system_manager->get_system<systems::Attribute_system>(ecs::System_type::Attributes);
	auto name = a.name;
	auto modifier = a.modifier;
	attribute_system->buy_units(string_to_attrib[name], entity, modifier * 100);
//	attribute_system->buy_attribute(string_to_attrib[name], entity, modifier);
}

sfg::Table::Ptr State_character_generator::create_attribute_table()
{
	auto table = sfg::Table::Create();
	table->SetColumnSpacings(20.0f);
	table->SetRowSpacings(10.0f);
	sf::Uint32 index{ 0 };
	for (auto attrib_value : m_attrib_values)
	{
		
		auto attrib = attrib_value.name;
		apply_attrib_modifier(attrib, attrib_value.modifier);
		auto val = std::to_string(attrib_value.base + attrib_value.modifier);

		table->Attach(sfg::Label::Create(attrib), rect_32{ 0, index, 1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
		table->Attach(sfg::Label::Create(val), rect_32{ 1, index, 1,1 }, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
		++index;
	}
	return table;
}

void State_character_generator::apply_attrib_modifier(const std::string& name, int modifier)
{
	if (name == "ST")
	{
		auto hp_itr = std::find_if(std::begin(m_attrib_values), std::end(m_attrib_values), [](const Attrib_value& attrib) {return attrib.name == "HP"; });
		hp_itr->base += modifier;
	}
	if (name == "IQ")
	{
		auto will_itr = std::find_if(std::begin(m_attrib_values), std::end(m_attrib_values), [](const Attrib_value& attrib) {return attrib.name == "Will"; });
		will_itr->base += modifier;
		auto per_itr = std::find_if(std::begin(m_attrib_values), std::end(m_attrib_values), [](const Attrib_value& attrib) {return attrib.name == "Per"; });
		per_itr->base += modifier;
	}
	if (name == "HT")
	{
		auto fp_itr = std::find_if(std::begin(m_attrib_values), std::end(m_attrib_values), [](const Attrib_value& attrib) {return attrib.name == "FP"; });
		fp_itr->base += modifier;
	}
}

void State_character_generator::read_attrib_modifiers(const std::string& race)
{
	for (auto& attrib : m_attrib_values)
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
		if (itr != m_attrib_values.end()) itr->modifier = mod;
/*		if (m_attrib_values.find(attribute) != m_attrib_values.end())
		{
			m_attrib_values[attribute].modifier = mod;
		}*/
	}
}

void State_character_generator::on_destroy()
{

}

void State_character_generator::update(const sf::Time& time)
{
	m_desktop.Update(time.asSeconds());
}

void State_character_generator::handle_sfml_event(sf::Event& e)
{
	m_desktop.HandleEvent(e);
}

void State_character_generator::activate()
{
	m_root_widget->Show(true);
}

void State_character_generator::deactivate()
{
	m_root_widget->Show(false);
}

void State_character_generator::draw()
{
	m_context->m_wind->get_renderwindow()->clear();
	m_sfgui.Display(*m_context->m_wind->get_renderwindow());
}

void State_character_generator::to_game()
{
	m_context->m_state_manager->switch_to(Game_state::Game);
}