#include "character_sheet.h"
#include "shared_context.h"
#include "character.h"
#include "attributes.h"
#include "container.h"
#include "position.h"
#include "ecs_types.h"
#include "state_game.h"
#include "component.h"
#include "drawable.h"
#include "map_data.h"
#include "systems.h"
#include "inventory_system.h"

#include <sstream>
#include <iomanip>

Character_sheet::Character_sheet(Shared_context* context) : m_context{ context }
{
	m_em = m_context->m_entity_manager;
	m_character = m_em->get_component<ecs::Component<Character>>(ecs::Component_type::Character);
	m_attributes = m_em->get_component <ecs::Component<Attributes>>(ecs::Component_type::Attributes);
	m_container = m_em->get_component<ecs::Component<Container>>(ecs::Component_type::Container);
	m_party = std::move(get_party_data());
	auto outer = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	auto inner = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	inner->SetId("inner_box");
	inner->SetSpacing(20.0f);
	outer->Pack(inner, false);
	auto combobox = sfg::ComboBox::Create();
	std::for_each(std::cbegin(m_party), std::cend(m_party), [&combobox](const Party_member& member) {combobox->AppendItem(member.name); });
	combobox->SetId("party_combobox");
	combobox->GetSignal(sfg::ComboBox::OnSelect).Connect([this]() {on_select(); });
	inner->Pack(combobox, false);
	inner->Pack(sfg::Separator::Create(), false);
	auto sheet = sfg::Notebook::Create();
	sheet->AppendPage(create_character_page(), sfg::Label::Create("Character"));
	sheet->AppendPage(create_attribute_page(), sfg::Label::Create("Attributes"));
	sheet->AppendPage(create_inventory_page(), sfg::Label::Create("Inventory"));
	inner->Pack(sheet, false);

	m_root = outer;
}

sfg::Widget::Ptr Character_sheet::create_character_page() const
{
	auto table = sfg::Table::Create();
	table->SetColumnSpacings(20.0f);
	table->SetRowSpacings(20.0f);
	sfg::Label::Ptr name_label = sfg::Label::Create("name");
	name_label->SetId("name_label");
	sfg::Label::Ptr gender_label = sfg::Label::Create("gender");
	gender_label->SetId("gender_label");
	auto race_label = sfg::Label::Create("race");
	race_label->SetId("race_label");
	auto cp_label = sfg::Label::Create("");
	cp_label->SetId("cp_label");
	auto sm_label = sfg::Label::Create("");
	sm_label->SetId("sm_label");
	table->Attach(sfg::Label::Create("Name"), sf::Rect<sf::Uint32>{0, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(name_label, sf::Rect<sf::Uint32>{1, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Gender"), sf::Rect<sf::Uint32>{0, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(gender_label, sf::Rect<sf::Uint32>{1, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Race"), sf::Rect<sf::Uint32>{0, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(race_label, sf::Rect<sf::Uint32>{1, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Available character points"), sf::Rect<sf::Uint32>{0, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(cp_label, sf::Rect<sf::Uint32>{1, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Size Modifier"), sf::Rect<sf::Uint32>{0, 4, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sm_label, sf::Rect<sf::Uint32>{1, 4, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	return table;
}

sfg::Widget::Ptr Character_sheet::create_attribute_page() const
{
	auto table = sfg::Table::Create();
	table->SetColumnSpacings(20.0f);
	table->SetRowSpacings(20.0f);

	auto st_label = sfg::Label::Create("");
	st_label->SetId("st_label");
	auto dx_label = sfg::Label::Create("");
	dx_label->SetId("dx_label");
	auto iq_label = sfg::Label::Create("");
	iq_label->SetId("iq_label");
	auto ht_label = sfg::Label::Create("");
	ht_label->SetId("ht_label");
	auto hp_label = sfg::Label::Create("");
	hp_label->SetId("hp_label");
	auto will_label = sfg::Label::Create("");
	will_label->SetId("will_label");
	auto per_label = sfg::Label::Create("");
	per_label->SetId("per_label");
	auto fp_label = sfg::Label::Create("");
	fp_label->SetId("fp_label");
	auto bs_label = sfg::Label::Create("");
	bs_label->SetId("bs_label");
	auto bm_label = sfg::Label::Create("");
	bm_label->SetId("bm_label");
	auto em_label = sfg::Label::Create("");
	em_label->SetId("em_label");
	auto dodge_label = sfg::Label::Create("");
	dodge_label->SetId("dodge_label");

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

	return table;
}

sfg::Widget::Ptr Character_sheet::create_inventory_page() 
{
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
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
	return box;
}

sfg::Widget::Ptr Character_sheet::get_charsheet() const
{
	
	//party_combobox->SelectItem(0);
	return m_root;
}

std::vector<Party_member> Character_sheet::get_party_data() const
{
	std::vector<Party_member> party;
	std::transform(std::cbegin(m_context->m_party), std::cend(m_context->m_party), std::back_inserter(party), [this](ecs::Entity_id entity) {return get_party_member(entity); });
	return party;
}

Party_member Character_sheet::get_party_member(ecs::Entity_id entity) const
{
	Party_member member;
	auto character_index = m_em->get_index(ecs::Component_type::Character, entity);
	auto attributes_index = m_em->get_index(ecs::Component_type::Attributes, entity);
	auto container_index = m_em->get_index(ecs::Component_type::Container, entity);
	if (!character_index || !attributes_index || !container_index) std::cout << "could not find index for needed component: " << entity << "\n";
	member.id = entity;
	member.character = &m_character->m_data[*character_index];
	member.attributes = &m_attributes->m_data[*attributes_index];
	member.container = &m_container->m_data[*container_index];
	member.name = member.character->name;
	return member;
}

std::string Character_sheet::get_attribute(Attributes* attribs, attributes::Attrib attrib) const
{
	auto val = static_cast<double> (attributes::get_total_value(attribs->transactions, attrib) / 100.0);
	auto cost = "[" + std::to_string (attributes::get_spent_points(attribs->transactions, attrib)) + "]";
	cost.insert(0, 7 - cost.size(), ' ');
	std::stringstream ss;
	ss << std::right << std::setw(5) << std::setfill(' ') << std::fixed << std::setprecision(2) << val << cost;
	return ss.str();
}

sfg::Label::Ptr find_label(const std::string& id)
{
	return std::dynamic_pointer_cast<sfg::Label>(sfg::Widget::GetWidgetById(id));
}

void Character_sheet::on_drop_item()
{
	auto inventory_system = m_context->m_system_manager->get_system<systems::Inventory_system>(ecs::System_type::Inventory);
	std::for_each(std::begin(m_inventory), std::end(m_inventory), [this, inventory_system](Inventory_item& item)
		{
			if (item.button != nullptr && item.button->IsActive() && item.entity.has_value())
			{
				auto member = get_party_member(m_current_member);
				ecs::Entity_id entity = item.entity.value();
				item.button->ClearImage();
				inventory_system->drop_item(member.id, item.entity.value());
				// don't need entity ref anymore, remove from inventory item
				item.entity = std::nullopt;
			}
		});
}

void Character_sheet::on_equip()
{
	std::for_each(std::begin(m_inventory), std::end(m_inventory), [this](Inventory_item& item)
		{
			if (item.button != nullptr && item.button->IsActive() && item.entity.has_value())
			{
				std::cout << "equipping item " << item.entity.value() << "\n";
			}
		});
}

void Character_sheet::on_use_item()
{
	auto entity_itr = std::find_if(std::cbegin(m_inventory), std::cend(m_inventory), [this](const Inventory_item& item) {return item.button != nullptr && item.button->IsActive() && item.entity.has_value(); });
	if (entity_itr != std::cend(m_inventory))
	{
		auto ent = entity_itr->entity.value();
		m_context->m_active_object = ent;
		std::cout << "selected obj " << ent <<"\n";
	}
}

void Character_sheet::on_select() 
{
	auto combobox = std::dynamic_pointer_cast<sfg::ComboBox> (sfg::Widget::GetWidgetById("party_combobox"));
	auto name = combobox->GetSelectedText().toAnsiString();
	//auto em = m_context->m_entity_manager;
	
	auto party_member = std::find_if(std::begin(m_party), std::end(m_party), [name](Party_member member) {return member.name == name; });
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
			if (opt_ent) item.button->SetImage(sfg::Image::Create(image));
			inventory_table->Attach(item.button, sf::Rect<sf::Uint32>(j, i, 1, 1), sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
			m_inventory.push_back(item);
		}
	}
}

