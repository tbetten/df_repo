#include "character_sheet.h"
#include "shared_context.h"
#include "character.h"
#include "attributes.h"
#include "inventory.h"
#include "inventory_slot.h"
#include "container.h"
#include "position.h"
#include "item.h"
#include "ecs_types.h"
#include "state_game.h"
#include "component.h"
#include "drawable.h"
#include "map_data.h"
#include "systems.h"
#include "inventory_system.h"
#include "utils.h"
#include "resource_cache.h"

#include <sstream>
#include <iomanip>
#include <cassert>
#include <any>
#include <memory>

/*Party_member Character_sheet::get_party_member (ecs::Entity_id entity) const
{
	Party_member member;
	auto character_index = m_em->get_index (ecs::Component_type::Character, entity);
	auto attributes_index = m_em->get_index (ecs::Component_type::Attributes, entity);
	auto container_index = m_em->get_index (ecs::Component_type::Container, entity);
	if (!character_index || !attributes_index || !container_index) std::cout << "could not find index for needed component: " << entity << "\n";
	member.id = entity;
	member.character = &m_character->m_data [*character_index];
	member.attributes = &m_attributes->m_data [*attributes_index];
	member.container = &m_container->m_data [*container_index];
	member.name = member.character->name;
	return member;
}*/

Party_member::Party_member (ecs::Entity_id entity, ecs::Entity_manager* em)
{
	id = entity;
	m_em = em;
	character = em->get_data<ecs::Component<Character>> (ecs::Component_type::Character, entity);
	attributes = em->get_data<ecs::Component<Attributes>> (ecs::Component_type::Attributes, entity);
	inventory = em->get_data<ecs::Component<Inventory>> (ecs::Component_type::Inventory, entity);
	drawable = m_em->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	name = character->name;
}

void on_tab_change ()
{
	auto sheet = find_widget<sfg::Notebook> ("charsheet");  // if sfg::Notebook::Ptr& is passed, GetCurrentPage always returns 0
	auto desc_frame = find_widget<sfg::Frame> ("description_frame");
	desc_frame->Show (sheet->GetCurrentPage() == 2);
}

Character_sheet::Character_sheet (Shared_context* context, sfg::Desktop* desktop) : m_context { context }, m_desktop { desktop }
{
	m_em = m_context->m_entity_manager;
	m_em->get_messenger ()->bind ("inventory_changed", [this] (const std::any& val) {on_inventory_changed (val); });

	auto box = sfg::Box::Create (sfg::Box::Orientation::VERTICAL);
	auto outer_box = sfg::Box::Create (sfg::Box::Orientation::HORIZONTAL);
	outer_box->Pack (box);
	box->SetSpacing (20.0f);

	auto combobox = sfg::ComboBox::Create ();
	combobox->SetId ("party_combobox");
	combobox->GetSignal (sfg::ComboBox::OnSelect).Connect ([this] (){on_select (); });

	populate_party ();

	box->Pack (combobox, false);

	auto sheet = sfg::Notebook::Create ();
	sheet->GetSignal (sfg::Notebook::OnTabChange).Connect ([] (){on_tab_change (); });
	sheet->SetId ("charsheet");
	sheet->SetRequisition (sf::Vector2f { 200.0f, 100.0f });
	sheet->AppendPage (create_character_page (m_party[0]), sfg::Label::Create("Character"));
	sheet->AppendPage (create_attribute_page (m_party [0]), sfg::Label::Create ("Attributes"));
	sheet->AppendPage (create_inventory_page (m_party [0]), sfg::Label::Create ("Inventory"));
	

	box->Pack (sheet, false, true);

	m_root = outer_box;
}



void Character_sheet::populate_party ()
{
	auto combobox = find_widget<sfg::ComboBox> ("party_combobox");
	auto party = m_context->m_party;
	std::transform (std::cbegin (party), std::cend (party), std::back_inserter (m_party), [this] (const ecs::Entity_id entity)
		{
			Party_member m { entity, m_em };
			//add_sheets (m);
			return m;
		});
	//std::for_each (std::begin (party), std::end (party), [this] (Party_member& member){add_sheets (member); });
	//m_party = get_party_data ();
	std::for_each (std::cbegin (m_party), std::cend (m_party), [&combobox] (const Party_member& member) {combobox->AppendItem (member.name); });
}

/*void Character_sheet::add_sheets (Party_member& member)
{
	member.character_view = create_character_page (member);
	member.attributes_view = create_attribute_page (member);
	member.inventory_view = create_inventory_page (member);
}*/

/*
std::vector<Party_member> Character_sheet::get_party_data () const
{
	std::vector<Party_member> party;
	std::transform (std::cbegin (m_context->m_party), std::cend (m_context->m_party), std::back_inserter (party), [this] (ecs::Entity_id entity) {return get_party_member (entity); });
	return party;
}
*/

void add_to_table (sfg::Table::Ptr table, sfg::Widget::Ptr widget, int x, int y)
{
	table->Attach (widget, sf::Rect<sf::Uint32>{static_cast<sf::Uint32>(x), static_cast<sf::Uint32>(y), 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
}

sfg::Widget::Ptr Character_sheet::create_character_page(const Party_member& m) const
{
	auto table = sfg::Table::Create();
	table->SetColumnSpacings(20.0f);
	table->SetRowSpacings(20.0f);
	sfg::Label::Ptr name_label = sfg::Label::Create ("");
	name_label->SetId ("name_label_");// +std::to_string (m.id));
	sfg::Label::Ptr gender_label = sfg::Label::Create ("");
	gender_label->SetId ("gender_label_");// +std::to_string (m.id));
	auto race_label = sfg::Label::Create("");
	race_label->SetId ("race_label_");// +std::to_string (m.id));
	auto cp_label = sfg::Label::Create("");
	cp_label->SetId ("cp_label_");// +std::to_string (m.id));
	auto sm_label = sfg::Label::Create("");
	sm_label->SetId ("sm_label_");// +std::to_string (m.id));
	
	add_to_table (table, sfg::Label::Create ("Name"), 0, 0);
	add_to_table (table, name_label, 1, 0);
	add_to_table (table, sfg::Label::Create ("Gender"), 0, 1);
	add_to_table (table, gender_label, 1, 1);
	add_to_table (table, sfg::Label::Create ("Race"), 0, 2);
	add_to_table (table, race_label, 1, 2);
	add_to_table (table, sfg::Label::Create ("Available character points"), 0, 3);
	add_to_table (table, cp_label, 1, 3);
	add_to_table (table, sfg::Label::Create ("Size Modifier"), 0, 4);
	add_to_table (table, sm_label, 1, 4);
	//table->Attach(sfg::Label::Create("Name"), sf::Rect<sf::Uint32>{0, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(name_label, sf::Rect<sf::Uint32>{1, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(sfg::Label::Create("Gender"), sf::Rect<sf::Uint32>{0, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(gender_label, sf::Rect<sf::Uint32>{1, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(sfg::Label::Create("Race"), sf::Rect<sf::Uint32>{0, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(race_label, sf::Rect<sf::Uint32>{1, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(sfg::Label::Create("Available character points"), sf::Rect<sf::Uint32>{0, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(cp_label, sf::Rect<sf::Uint32>{1, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(sfg::Label::Create("Size Modifier"), sf::Rect<sf::Uint32>{0, 4, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	//table->Attach(sm_label, sf::Rect<sf::Uint32>{1, 4, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	//auto itr = std::find_if (std::cbegin (m_party), std::cend (m_party), [entity] (const Party_member& m){return entity == m.id; });
	populate_character_page (m);
	return table;
}

void populate_character_page (const Party_member& m)
{
	std::string s = std::to_string (m.id);
	auto label = find_widget ("name_label_");// +s);
	label->SetText (m.character->name);
	label = find_widget ("gender_label_");// +s);
	label->SetText (m.character->gender == Gender::Male ? "Male" : "Female");
	label = find_widget ("race_label_");// +s);
	label->SetText (race_to_string (m.character->race));
	label = find_widget ("cp_label_");// +s);
	label->SetText (m.character->character_points);
	label = find_widget ("sm_label_");// +s);
	label->SetText ("");
}

sfg::Widget::Ptr Character_sheet::create_attribute_page(const Party_member& m) const
{
	auto table = sfg::Table::Create();
	table->SetColumnSpacings(20.0f);
	table->SetRowSpacings(20.0f);

	auto s = std::to_string (m.id);
	auto st_label = sfg::Label::Create("");
	st_label->SetId ("st_label_");// +s);
	auto dx_label = sfg::Label::Create("");
	dx_label->SetId ("dx_label_");// +s);
	auto iq_label = sfg::Label::Create("");
	iq_label->SetId ("iq_label_");// +s);
	auto ht_label = sfg::Label::Create("");
	ht_label->SetId ("ht_label_");// +s);
	auto hp_label = sfg::Label::Create("");
	hp_label->SetId ("hp_label_");// +s);
	auto will_label = sfg::Label::Create("");
	will_label->SetId ("will_label_");// +s);
	auto per_label = sfg::Label::Create("");
	per_label->SetId ("per_label_");// +s);
	auto fp_label = sfg::Label::Create("");
	fp_label->SetId ("fp_label_");// +s);
	auto bs_label = sfg::Label::Create("");
	bs_label->SetId ("bs_label_");// +s);
	auto bm_label = sfg::Label::Create("");
	bm_label->SetId ("bm_label_");// +s);
	auto em_label = sfg::Label::Create ("");
	em_label->SetId ("em_label_");// +s);
	auto dodge_label = sfg::Label::Create("");
	dodge_label->SetId ("dodge_label_");// +s);

	table->Attach(sfg::Label::Create("ST"), sf::Rect<sf::Uint32>{0, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("DX"), sf::Rect<sf::Uint32>{0, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("IQ"), sf::Rect<sf::Uint32>{0, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("HT"), sf::Rect<sf::Uint32>{0, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	table->Attach(st_label, sf::Rect<sf::Uint32>{1, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(dx_label, sf::Rect<sf::Uint32>{1, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(iq_label, sf::Rect<sf::Uint32>{1, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(ht_label, sf::Rect<sf::Uint32>{1, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	table->Attach(sfg::Label::Create("HP"), sf::Rect<sf::Uint32>{2, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Will"), sf::Rect<sf::Uint32>{2, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Per"), sf::Rect<sf::Uint32>{2, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("FP"), sf::Rect<sf::Uint32>{2, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	table->Attach(hp_label, sf::Rect<sf::Uint32>{3, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(will_label, sf::Rect<sf::Uint32>{3, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(per_label, sf::Rect<sf::Uint32>{3, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(fp_label, sf::Rect<sf::Uint32>{3, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	table->Attach(sfg::Label::Create("Basic speed"), sf::Rect<sf::Uint32>{4, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Basic move"), sf::Rect<sf::Uint32>{4, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Encumbered move"), sf::Rect<sf::Uint32>{4, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Dodge"), sf::Rect<sf::Uint32>{4, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	table->Attach(bs_label, sf::Rect<sf::Uint32>{5, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(bm_label, sf::Rect<sf::Uint32>{5, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(em_label, sf::Rect<sf::Uint32>{5, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(dodge_label, sf::Rect<sf::Uint32>{5, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);

	populate_attribute_page (m);
	return table;
}

void populate_attribute_page (const Party_member& m)
{
	auto [move, dodge] = attributes::get_encumbered_value (m.attributes->transactions, m.character->encumbrance);

	std::string id = std::to_string (m.id);
	auto label = find_widget ("st_label_");// +id);
	//using namespace attributes;
	using enum attributes::Attrib;
	label->SetText (get_attribute (m.attributes, ST));
	label = find_widget ("dx_label_");// +id);
	label->SetText (get_attribute (m.attributes, DX));
	label = find_widget ("iq_label_");// +id);
	label->SetText (get_attribute (m.attributes, IQ));
	label = find_widget ("ht_label_");// +id);
	label->SetText (get_attribute (m.attributes, HT));
	label = find_widget ("hp_label_");// +id);
	label->SetText (get_attribute (m.attributes, HP));
	label = find_widget ("will_label_");// +id);
	label->SetText (get_attribute (m.attributes, Will));
	label = find_widget ("per_label_");// +id);
	label->SetText (get_attribute (m.attributes, Per));
	label = find_widget ("fp_label_");// +id);
	label->SetText (get_attribute (m.attributes, FP));
	label = find_widget ("bs_label_");// +id);
	label->SetText (get_attribute (m.attributes, BS));
	label = find_widget ("bm_label_");// +id);
	label->SetText (get_attribute (m.attributes, BM));
	label = find_widget ("em_label_");// +id);
	label->SetText (std::to_string (move));
	label = find_widget ("dodge_label_");// +id);
	label->SetText ("");
}

void draw_portrait (Drawable* d, sfg::Canvas::Ptr c)
{
	auto tex = d->composed_icon->getTexture ();
	auto sprite = sf::Sprite (tex);
	sprite.scale (sf::Vector2f { 3.0f, 3.0f });
	c->Bind ();
	c->Clear (sf::Color::Transparent);
	c->Draw (sprite);
	c->Display ();
	c->Unbind ();
}

sfg::Widget::Ptr Character_sheet::create_inventory_page(const Party_member& m)  
{
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	box->SetSpacing (5.0f);
	auto upper_box = sfg::Box::Create (sfg::Box::Orientation::HORIZONTAL);
	auto image = sfg::Canvas::Create ();
	image->SetRequisition (sf::Vector2f { 96.0f, 96.0f });
	image->SetId ("portrait");
	draw_portrait (m.drawable, image);
	upper_box->Pack (image, false, false);
	auto inventory_table = sfg::Table::Create ();
	inventory_table->SetRowSpacings (5.0f);
	inventory_table->SetId ("equipped_table");
	
	size_t i { 0 };
	size_t columns { 2 };
	size_t s = inventory::string_to_slot.size () - 1;
	for (auto& [slot_name, slot]  : inventory::string_to_slot)
	{
		if (slot_name == "None") continue;
		auto q = i / (s /columns);  // 0..4 -> 0 3..9 -> 1
		auto r = i % (s/columns);
		auto button = sfg::ToggleButton::Create ();
		button->SetRequisition (sf::Vector2f { 36.0f,36.0f });
		button->SetId ("inventory_slot_" + slot_name);
		add_to_table (inventory_table, button, q * columns, r );
		add_to_table (inventory_table, sfg::Label::Create (slot_name), q * columns + 1, r);
		++i;
	}

	upper_box->Pack (inventory_table);

	box->Pack (upper_box);
	box->Pack (sfg::Separator::Create ());
	auto table = sfg::Table::Create();
	table->SetId("inventory");
	box->Pack(table);
	auto inner_box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	auto drop_button = sfg::Button::Create("drop");
	drop_button->GetSignal(sfg::Widget::OnLeftClick).Connect([this]() {on_drop_item(); });
	inner_box->Pack(drop_button, false, false);
	auto equip_button = sfg::Button::Create("equip");
	equip_button->GetSignal(sfg::Widget::OnLeftClick).Connect([this]() {on_equip(); });
	inner_box->Pack(equip_button, false, false);
	auto use_button = sfg::Button::Create("use");
	use_button->GetSignal(sfg::Widget::OnLeftClick).Connect([this]() {on_use_item(); });
	inner_box->Pack(use_button, false, false);
	box->Pack(inner_box, false, false);

	populate_inventory_page (m);
	return box;
}

void set_button_image (ecs::Entity_manager* em, ecs::Entity_id entity, sfg::ToggleButton::Ptr button)
{
	auto drawable = em->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	auto image = drawable->composed_icon->getTexture ().copyToImage ();
	button->ClearImage ();
	button->SetImage (sfg::Image::Create (image));
}

void Character_sheet::populate_inventory_page (const Party_member& m)
{
	auto rows = (m.inventory->bag.contents.size() / 10) + 1;
	auto table = find_widget<sfg::Table> ("inventory");
	auto cells = table->GetChildren ();
	auto table_rows = cells.size() / 10;
	if (rows > table_rows)
	{
		for (unsigned int i = 0; i < rows - table_rows; ++i)
		{
			for (auto j = 0; j < 10; ++j)
			{
				auto button = sfg::ToggleButton::Create ();
				button->SetRequisition (sf::Vector2f { 36.0f, 36.0f });
				//button->GetSignal (sfg::Widget::OnRightClick).Connect ([this, table, index = i * 10 + j] (){show_description (table, index); });
				button->GetSignal (sfg::Widget::OnMouseEnter).Connect ([this, index = i * 10 + j] (){show_description (index, false); });
				add_to_table (table, button, j, table_rows + i);
			}
		}
	}
	size_t index { 0 };
	std::for_each (std::begin (cells), std::end (cells), [] (sfg::Widget::Ptr cell){std::dynamic_pointer_cast<sfg::ToggleButton>(cell)->ClearImage (); });
	for (auto entity : m.inventory->bag.contents)
	{
		set_button_image (m.m_em, entity, std::dynamic_pointer_cast<sfg::ToggleButton>(cells.at (index)));
		++index;
	}

	auto equipped_table = find_widget<sfg::Table> ("equipped_table");
	auto equipped_slots = equipped_table->GetChildren ();
	// the buttons AND the labels are children of the table. Only clear the image if the 
	// cast to sfg::ToggleButton succeeds.
	std::ranges::for_each (equipped_slots, [] (sfg::Widget::Ptr slot)
		{
			if (auto button = std::dynamic_pointer_cast<sfg::ToggleButton>(slot))
			{
				button->ClearImage();
			}
		});

	auto equipped = m.inventory->inventory;
	index = 0;
	for (auto& slot : equipped)
	{
		if (!slot)
		{
			++index;
			continue;
		}
		auto i = static_cast<inventory::Inventory_slot>(index);
		auto n = std::find_if (std::begin (inventory::string_to_slot), std::end (inventory::string_to_slot), [i] (std::pair<std::string, inventory::Inventory_slot> p){return p.second == i; });
		auto name = "inventory_slot_" + n->first;
		auto button = find_widget<sfg::ToggleButton> (name);
		button->GetSignal (sfg::Widget::OnMouseEnter).Connect ([this, index] (){show_description (index, true); });
		set_button_image (m.m_em, slot.value (), find_widget<sfg::ToggleButton> (name));
		++index;
	}

	auto portrait = find_widget<sfg::Canvas> ("portrait");
	draw_portrait (m.drawable, portrait);
}

void Character_sheet::destroy (std::weak_ptr<sfg::Window> w)
{
	/*auto w = sfg::Context::Get ().GetActiveWidget ();
	while (w->GetName () != "Window")
	{
		w = w->GetParent ();
	}*/
	m_remove.push_back(w);
}

void Character_sheet::show_description (size_t index, bool equipped)
{
	//auto cells = table->GetChildren ();
	//auto clicked = cells.at (index);
	auto inv = m_em->get_data<ecs::Component<Inventory>> (ecs::Component_type::Inventory, m_current_member);
	std::string desc { "" };
	std::optional<ecs::Entity_id> item { std::nullopt };
	if (equipped)
	{
		item = inv->inventory.at (index);
	}
	else
	{
		if (index < inv->bag.contents.size ()) item = inv->bag.contents.at (index);
	}
	if (item)
	{
		auto item_data = m_em->get_data<ecs::Component<Item>> (ecs::Component_type::Item, item.value ());
		desc = item_data->description;
	}
	auto desc_label = find_widget ("item_description");
	desc_label->SetText (desc);
/*	if (index < inv->bag.contents.size ())
	{
		if (m_description_popup)
		{
			m_description_popup->Show (true);
		}
		else
		{
			m_description_popup = sfg::Window::Create (sfg::Window::Style::TOPLEVEL | sfg::Window::Style::CLOSE);
			m_description_popup->GetSignal (sfg::Window::OnCloseButton).Connect ([this] (){m_description_popup->Show (false); });
			auto text = sfg::Label::Create ();
			text->SetRequisition({ 200.0f,0.0f });
			text->SetLineWrap (true);
			text->SetId ("item_description");
			m_description_popup->Add (text);
			m_desktop->Add (m_description_popup);
		}
		auto text = find_widget ("item_description");
		text->SetText (desc);
	}*/
}

sfg::Widget::Ptr Character_sheet::get_charsheet() const
{
	return m_root;
}

std::string get_attribute(Attributes* attribs, attributes::Attrib attrib)
{
	auto val = static_cast<double> (attributes::get_total_value(attribs->transactions, attrib) / 100.0);
	auto cost = "[" + std::to_string (attributes::get_spent_points(attribs->transactions, attrib)) + "]";
	cost.insert(0, 7 - cost.size(), ' ');
	std::stringstream ss;
	ss << std::right << std::setw(5) << std::setfill(' ') << std::fixed << std::setprecision(2) << val << cost;
	return ss.str();
}


void Character_sheet::on_drop_item()
{
	auto inventory_system = m_context->m_system_manager->get_system<systems::Inventory_system>(ecs::System_type::Inventory);
	auto inventory = find_widget<sfg::Table> ("inventory");
	auto member = std::find_if (std::cbegin (m_party), std::cend (m_party), [this] (const Party_member& m){return m_current_member == m.id; });
	auto children = inventory->GetChildren ();
	auto size = member->inventory->bag.contents.size ();
	for (int index = size -1; index >=0; --index)
	{
		auto button = std::dynamic_pointer_cast<sfg::ToggleButton>(children.at (index));
		if (button->IsActive ())
		{
			button->ClearImage ();
			button->SetActive (false);
			auto item = member->inventory->bag.contents.at (index);
			inventory_system->drop_item (member->id, item);
		}
	}
}

void Character_sheet::on_equip()
{
	auto m = std::ranges::find_if (m_party, [this] (const Party_member& m){return m.id == m_current_member; });
	auto inventory_system = m_context->m_system_manager->get_system<systems::Inventory_system> (ecs::System_type::Inventory);
	// first unequip
	auto& equipped = find_widget<sfg::Table> ("equipped_table")->GetChildren ();
	auto is_active = [] (const sfg::Widget::Ptr p)
	{
		auto t = std::dynamic_pointer_cast<sfg::ToggleButton>(p);
		return t == nullptr ? false : t->IsActive ();
	};
	std::vector<ecs::Entity_id> equip_list;
	for (auto current = std::ranges::find_if (equipped, is_active); current != std::end(equipped); current= std::find_if(current, std::end(equipped), [] (const sfg::Widget::Ptr p){auto t = std::dynamic_pointer_cast<sfg::ToggleButton>(p); return t == nullptr ? false : t->IsActive ();}))
	{

		auto button = std::dynamic_pointer_cast<sfg::ToggleButton>(*current);
		button->SetActive (false);
		button->ClearImage ();
		auto i = std::distance (std::begin (equipped), current);
		if (auto entity = m->inventory->inventory.at (i / 2 + 1))
		{
			equip_list.push_back (entity.value ());
			//inventory_system->unequip_item (m_current_member, entity.value());
		}
		current++;
	}
	inventory_system->unequip_items (m_current_member, equip_list);
	equip_list.clear ();
	// then equip
	auto inventory = find_widget<sfg::Table> ("inventory")->GetChildren();
	//auto inv_active = [] (const sfg::Widget::Ptr p){auto i = std::dynamic_pointer_cast<sfg::ToggleButton>(p); return i->IsActive (); };

	for (auto current = std::ranges::find_if (inventory, is_active); current != std::end (inventory); current = std::find_if (current, std::end (inventory), is_active))
	{
		auto button = std::dynamic_pointer_cast<sfg::ToggleButton>(*current);
		button->SetActive (false);
		size_t index = std::distance (std::begin (inventory), current);
		ecs::Entity_id item = m->inventory->bag.contents.at (index);
		if (m_em->has_component (item, ecs::Component_type::Equippable))
		{
			equip_list.push_back (item);
			//inventory_system->equip_item (m_current_member, item);
		}
		current++;
	}
	inventory_system->equip_items (m_current_member, equip_list);
	populate_inventory_page (*m);
}

void Character_sheet::on_use_item()
{
	auto inv = find_widget<sfg::Table> ("inventory")->GetChildren();
	auto button_itr = std::find_if (std::cbegin (inv), std::cend (inv), [] (const sfg::Widget::Ptr& p){auto button = std::dynamic_pointer_cast<sfg::ToggleButton>(p); return button->IsActive (); });
	if (button_itr == std::cend(inv)) return;
	auto& inventory_bag = m_em->get_data<ecs::Component<Inventory>> (ecs::Component_type::Inventory, m_current_member)->bag.contents;
	auto item = inventory_bag.at (std::distance (std::cbegin (inv), button_itr));
	auto item_data = m_em->get_data<ecs::Component<Item>> (ecs::Component_type::Item, item);
	if (!item_data->useable || !m_em->has_component(item, ecs::Component_type::Drawable)) return;
	auto drawable = m_em->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, item);
	auto image = drawable->composed_icon->getTexture ().copyToImage ();
	auto c = sf::Cursor {};
	c.loadFromPixels (image.getPixelsPtr (), sf::Vector2u { 32,32 }, sf::Vector2u { 0,0 });
	m_context->m_active_object = item;
	std::cout << "selected obj " << item << "\n";

/*	auto entity_itr = std::find_if(std::cbegin(m_inventory), std::cend(m_inventory), [this](const Inventory_item& item) {return item.button != nullptr && item.button->IsActive() && item.entity.has_value(); });
	if (entity_itr != std::cend(m_inventory))
	{
		auto ent = entity_itr->entity.value();
		m_context->m_active_object = ent;
		std::cout << "selected obj " << ent <<"\n";
	}*/
}

void Character_sheet::on_inventory_changed (const std::any& val)
{
	ecs::Entity_id entity = std::any_cast<ecs::Entity_id>(val);
	auto party_itr = std::find_if (std::cbegin (m_party), std::cend (m_party), [entity] (const Party_member& m){return m.id == entity; });
	populate_inventory_page (*party_itr);
}

void Character_sheet::on_select() 
{
	auto combobox = find_widget<sfg::ComboBox> ("party_combobox");//  std::dynamic_pointer_cast<sfg::ComboBox> (sfg::Widget::GetWidgetById ("party_combobox"));
	auto name = combobox->GetSelectedText().toAnsiString();
	auto notebook = find_widget<sfg::Notebook> ("charsheet");
	auto itr = std::find_if (std::cbegin (m_party), std::cend (m_party), [name] (const Party_member& p){return name == p.name; });
	m_current_member = itr->id;

	auto current = notebook->GetCurrentPage ();
	populate_character_page (*itr);
	populate_attribute_page (*itr);
	populate_inventory_page (*itr);
}

