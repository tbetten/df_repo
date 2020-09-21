#include "character_sheet.h"
#include "shared_context.h"
#include "character.h"
#include "attributes.h"
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
	container = em->get_data<ecs::Component<Container>> (ecs::Component_type::Container, entity);
	drawable = m_em->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	name = character->name;
}


Character_sheet::Character_sheet (Shared_context* context) : m_context { context }
{
	m_em = m_context->m_entity_manager;
	m_character = m_em->get_component<ecs::Component<Character>> (ecs::Component_type::Character);
	m_attributes = m_em->get_component <ecs::Component<Attributes>> (ecs::Component_type::Attributes);
	m_container = m_em->get_component<ecs::Component<Container>> (ecs::Component_type::Container);

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
	sheet->SetId ("charsheet");
	sheet->SetRequisition (sf::Vector2f { 200.0f, 100.0f });
	std::cout << "address of sheet: " << sheet << "\n";
	sheet->AppendPage (create_character_page (m_party[0]), sfg::Label::Create("Character"));
	sheet->AppendPage (create_attribute_page (m_party [0]), sfg::Label::Create ("Attributes"));
	sheet->AppendPage (create_inventory_page (m_party [0]), sfg::Label::Create ("Inventory"));
//	m_sheet_indices [Sheet_view::Character_view] = sheet->AppendPage (sfg::Box::Create(), sfg::Label::Create ("foo 1"));
//	m_sheet_indices [Sheet_view::Attributes_view] = sheet->AppendPage (sfg::Box::Create(), sfg::Label::Create ("foo 2"));
//	m_sheet_indices [Sheet_view::Inventory_view] = sheet->AppendPage (sfg::Box::Create(), sfg::Label::Create ("foo 3"));


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
	
	auto head_slot = sfg::ToggleButton::Create ();
	head_slot->SetRequisition (sf::Vector2f { 36.0f,36.0f });
	head_slot->SetId ("inventory_slot_head");

	auto necklace_slot = sfg::ToggleButton::Create ();
	necklace_slot->SetRequisition (sf::Vector2f { 36.0f,36.0f });
	necklace_slot->SetId ("inventory_slot_necklace");

	auto torso_slot = sfg::ToggleButton::Create ();
	torso_slot->SetRequisition (sf::Vector2f { 36.0f,36.0f });
	torso_slot->SetId ("inventory_slot_torso");

	auto cloak_slot = sfg::ToggleButton::Create ();
	cloak_slot->SetRequisition (sf::Vector2f (36.0f, 36.0f));
	cloak_slot->SetId ("inventory_slot_cloak");

	auto hands_slot = sfg::ToggleButton::Create ();
	hands_slot->SetRequisition (sf::Vector2f { 36.0f,36.0f });
	hands_slot->SetId ("inventory_slot_hands");

	auto ring_left_slot = sfg::ToggleButton::Create ();
	ring_left_slot->SetRequisition (sf::Vector2f { 36.0f,36.0f });
	ring_left_slot->SetId ("inventory_slot_ring_left");

	auto ring_right_slot = sfg::ToggleButton::Create ();
	ring_right_slot->SetRequisition (sf::Vector2f (36.0f, 36.0f));
	ring_right_slot->SetId ("inventory_slot_ring_right");

	auto feet_slot = sfg::ToggleButton::Create ();
	feet_slot->SetRequisition (sf::Vector2f { 36.0f,36.0f });
	feet_slot->SetId ("inventory_slot_feet");

	add_to_table (inventory_table, head_slot, 0, 0);
	add_to_table (inventory_table, sfg::Label::Create ("Head"), 1, 0);
	add_to_table (inventory_table, necklace_slot, 0, 1);
	add_to_table (inventory_table, sfg::Label::Create ("Necklace"), 1, 1);
	add_to_table (inventory_table, torso_slot, 0, 2);
	add_to_table (inventory_table, sfg::Label::Create ("Torso"), 1, 2);
	add_to_table (inventory_table, cloak_slot, 0, 3);
	add_to_table (inventory_table, sfg::Label::Create ("Cloak"), 1, 3);
	add_to_table (inventory_table, hands_slot, 2, 0);
	add_to_table (inventory_table, sfg::Label::Create ("Hands"), 3, 0);
	add_to_table (inventory_table, ring_left_slot, 2, 1);
	add_to_table (inventory_table, sfg::Label::Create ("Left Ring"), 3, 1);
	add_to_table (inventory_table, ring_right_slot, 2, 2);
	add_to_table (inventory_table, sfg::Label::Create ("Right Ring"), 3, 2);
	add_to_table (inventory_table, feet_slot, 2, 3);
	add_to_table (inventory_table, sfg::Label::Create ("Feet"), 3, 3);
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
//	use_button->GetSignal(sfg::Widget::OnLeftClick).Connect([this]() {on_use_item(); });
	inner_box->Pack(use_button, false, false);
	box->Pack(inner_box, false, false);

	populate_inventory_page (m);
	return box;
}

void populate_inventory_page (const Party_member& m)
{
	auto rows = (m.container->contents.size() / 10) + 1;
	auto table = find_widget<sfg::Table> ("inventory");
	auto cells = table->GetChildren ();
	auto table_rows = cells.size() / 10;
	if (rows > table_rows)
	{
		for (auto i = 0; i < rows - table_rows; ++i)
		{
			for (auto j = 0; j < 10; ++j)
			{
				auto button = sfg::ToggleButton::Create ();
				button->SetRequisition (sf::Vector2f { 36.0f, 36.0f });
				add_to_table (table, button, j, table_rows + i);
			}
		}
	}
	size_t index { 0 };
	std::for_each (std::begin (cells), std::end (cells), [] (sfg::Widget::Ptr cell){std::dynamic_pointer_cast<sfg::ToggleButton>(cell)->ClearImage (); });
	for (auto entity : m.container->contents)
	{
		auto drawable = m.m_em->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
		auto image = drawable->composed_icon->getTexture ().copyToImage ();
		auto b = std::dynamic_pointer_cast<sfg::ToggleButton>(cells.at (index));
		b->ClearImage ();
		b->SetImage(sfg::Image::Create(image));
		++index;
	}
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
	auto size = member->container->contents.size ();
	for (int index = size -1; index >=0; --index)
	{
		auto button = std::dynamic_pointer_cast<sfg::ToggleButton>(children.at (index));
		if (button->IsActive ())
		{
			button->ClearImage ();
			button->SetActive (false);
			auto item = member->container->contents.at (index);
			inventory_system->drop_item (member->id, item);
		}
	}
}

void Character_sheet::on_equip()
{
	auto inventory = find_widget<sfg::Table> ("inventory")->GetChildren();
	auto button_itr = std::find_if (std::cbegin (inventory), std::cend (inventory), [] (const sfg::Widget::Ptr p){auto i = std::dynamic_pointer_cast<sfg::ToggleButton>(p); return i->IsActive (); });
	if (button_itr == std::cend (inventory)) return;
	size_t index = std::distance (std::cbegin (inventory), button_itr);
	auto m = std::find_if (std::cbegin (m_party), std::cend (m_party), [this] (const Party_member& m){return m.id == m_current_member; });
	ecs::Entity_id item = m->container->contents.at (index);
	if (m_em->has_component (item, ecs::Component_type::Item))
	{
		auto item_data = m_em->get_data<ecs::Component<Item>> (ecs::Component_type::Item, item);
		if (item_data->equippable)
		{
			auto inventory_system = m_context->m_system_manager->get_system<systems::Inventory_system> (ecs::System_type::Inventory);
			inventory_system->equip_item (m_current_member, item);

			auto portrait = find_widget<sfg::Canvas> ("portrait");
			draw_portrait (m->drawable, portrait);
		}
	}
}

/*void Character_sheet::on_use_item()
{
	auto entity_itr = std::find_if(std::cbegin(m_inventory), std::cend(m_inventory), [this](const Inventory_item& item) {return item.button != nullptr && item.button->IsActive() && item.entity.has_value(); });
	if (entity_itr != std::cend(m_inventory))
	{
		auto ent = entity_itr->entity.value();
		m_context->m_active_object = ent;
		std::cout << "selected obj " << ent <<"\n";
	}
}*/

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


/*
	if (party_member == std::end(m_party)) return;
	auto entity = party_member->id;
	m_current_member = entity;
	auto position = m_em->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
	party_member->coords = position->coords;
	auto [move, dodge] = attributes::get_encumbered_value(party_member->attributes->transactions, party_member->character->encumbrance);
	auto dodge_str = std::to_string(dodge / 100) + " (" + std::to_string(attributes::get_total_value(party_member->attributes->transactions, attributes::Attrib::Dodge) / 100) + ")";

	find_label("name_label")->SetText(party_member->name);
	auto gender_label = find_label("gender_label");
	party_member->character->gender == Gender::Male ? gender_label->SetText("Male") : gender_label->SetText("Female");
	find_label("race_label")->SetText(race_to_string(party_member->character->race));
	sf::String s = std::to_string(party_member->character->character_points);
	find_label("cp_label")->SetText(std::to_string(party_member->character->character_points));
	std::cout << "A " << std::string{ find_label("cp_label")->GetText() } << "\n";
	find_label("sm_label")->SetText(std::to_string(attributes::get_total_value(party_member->attributes->transactions, attributes::Attrib::SM) / 100));

	find_label("st_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::ST));
	find_label("dx_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::DX));
	find_label("iq_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::IQ));
	find_label("ht_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::HT));

	find_label("hp_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::HP));
	find_label("will_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::Will));
	find_label("per_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::Per));
	find_widget ("per_label")->SetText (get_attribute (party_member->attributes, attributes::Attrib::Per));
	find_label("fp_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::FP));

	find_label("bs_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::BS));
	find_label("bm_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::BM));
	//find_label("dodge_label")->SetText(get_attribute(party_member->attributes, attributes::Attrib::Dodge));
	find_label("em_label")->SetText(std::to_string(move));
	find_label("dodge_label")->SetText(dodge_str);

	auto inventory_size = party_member->container->contents.size();
	auto num_rows = inventory_size / 10 + 1;
	std::cout << inventory_size << "\t" << num_rows << "\n";
	auto inventory_table = std::dynamic_pointer_cast<sfg::Table> (sfg::Widget::GetWidgetById("inventory"));
	for (std::size_t i{ 0 }; i < num_rows; ++i)
	{
		for (int j{ 0 }; j < 10; ++j)
		{
			auto index = i * 10 + j;
			sf::Image image;
			std::optional<ecs::Entity_id> opt_ent{ std::nullopt };
			if (index < party_member->container->contents.size()) opt_ent = party_member->container->contents.at(index);
			if (opt_ent)
			{
				auto mgr = m_context->m_entity_manager;
				auto dr = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, *opt_ent);
				auto tex = dr->composed_icon->getTexture();
				image = tex.copyToImage();
				
			}
			auto b = sfg::ToggleButton::Create();
			b->SetRequisition(sf::Vector2f{ 36.0f, 36.0f });
			Inventory_item item{b, false, opt_ent};
			if (opt_ent)
			{
				item.button->ClearImage ();
				item.button->SetImage (sfg::Image::Create (image));
			}
			inventory_table->Attach(item.button, sf::Rect<sf::Uint32>(j, i, 1, 1), sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
			m_inventory.push_back(item);
		}
	}*/
}

