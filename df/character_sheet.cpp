#include "character_sheet.h"
#include "shared_context.h"
#include "character.h"
#include "attributes.h"
#include "container.h"
#include "ecs_types.h"
#include "state_game.h"
#include "component.h"
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
	auto cp_label = sfg::Label::Create("cp");
	cp_label->SetId("cp_label");
	table->Attach(sfg::Label::Create("Name"), sf::Rect<sf::Uint32>{0, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(name_label, sf::Rect<sf::Uint32>{1, 0, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Gender"), sf::Rect<sf::Uint32>{0, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(gender_label, sf::Rect<sf::Uint32>{1, 1, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Race"), sf::Rect<sf::Uint32>{0, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(race_label, sf::Rect<sf::Uint32>{1, 2, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(sfg::Label::Create("Available character points"), sf::Rect<sf::Uint32>{0, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	table->Attach(cp_label, sf::Rect<sf::Uint32>{1, 3, 1, 1}, sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
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

	return table;
}

sfg::Widget::Ptr Character_sheet::create_inventory_page() const
{
	auto table = sfg::Table::Create();
	table->SetId("inventory");
	return table;
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

std::string Character_sheet::get_attribute(Attributes* attribs, Attribute attrib) const
{
	auto& z = attribs->data[static_cast<int>(attrib)];
	//std::string buf("     ");
	float val = (z.base + z.bought) / 100.0f;
	auto cost = z.bought / static_cast<int>(z.units_per_point);
	std::stringstream ss;
	ss << std::right << std::setw(5) << std::setfill('*') << std::fixed << std::setprecision(2) << val << " [" << cost << "]";
	//auto res = std::to_chars(buf.data(), buf.data() + buf.size(), val, std::chars_format::fixed);
	//return buf;
	return ss.str();
}

sfg::Label::Ptr find_label(const std::string& id)
{
	return std::dynamic_pointer_cast<sfg::Label>(sfg::Widget::GetWidgetById(id));
}

void Character_sheet::on_select() 
{
	auto combobox = std::dynamic_pointer_cast<sfg::ComboBox> (sfg::Widget::GetWidgetById("party_combobox"));
	auto name = combobox->GetSelectedText().toAnsiString();
	//auto em = m_context->m_entity_manager;
	Party_member member{};
	auto party_member = std::find_if(std::cbegin(m_party), std::cend(m_party), [name](Party_member member) {return member.name == name; });
	if (party_member != std::cend(m_party)) member = *party_member;
	auto entity = member.id;

	find_label("name_label")->SetText(member.name);
	auto gender_label = find_label("gender_label");
	member.character->male ? gender_label->SetText("Male") : gender_label->SetText("Female");
	find_label("race_label")->SetText(race_to_string(member.character->race));
	find_label("cp_label")->SetText(std::to_string(member.character->character_points));

	find_label("st_label")->SetText(get_attribute(member.attributes, Attribute::ST));
	find_label("dx_label")->SetText(get_attribute(member.attributes, Attribute::DX));
	find_label("iq_label")->SetText(get_attribute(member.attributes, Attribute::IQ));
	find_label("ht_label")->SetText(get_attribute(member.attributes, Attribute::HT));

	find_label("hp_label")->SetText(get_attribute(member.attributes, Attribute::HP));
	find_label("will_label")->SetText(get_attribute(member.attributes, Attribute::Will));
	find_label("per_label")->SetText(get_attribute(member.attributes, Attribute::Per));
	find_label("fp_label")->SetText(get_attribute(member.attributes, Attribute::FP));

	auto inventory_size = member.container->contents.size();
	auto num_rows = inventory_size / 10 + 1;
	std::cout << inventory_size << "\t" << num_rows << "\n";
	auto inventory_table = std::dynamic_pointer_cast<sfg::Table> (sfg::Widget::GetWidgetById("inventory"));
	for (std::size_t i{ 0 }; i < num_rows; ++i)
	{
		for (int j{ 0 }; j < 10; ++j)
		{
			auto index = i * 10 + j;
			sf::Image im;
			im.loadFromFile("assets/sprite/long_sword_1_new.png");
			std::optional<ecs::Entity_id> opt_ent{ std::nullopt };
			if (index < member.container->contents.size()) opt_ent = member.container->contents.at(index);
			Inventory_item item{sfg::ToggleButton::Create(), false, opt_ent};
			if (opt_ent) item.button->SetImage(sfg::Image::Create(im));
			inventory_table->Attach(item.button, sf::Rect<sf::Uint32>(j, i, 1, 1), sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
			m_inventory.push_back(item);
		}
		auto row = i / inventory_size;
		auto column = i % inventory_size;
		inventory_table->Attach(sfg::ToggleButton::Create(), sf::Rect<sf::Uint32>(column, row, 1, 1), sfg::Table::AttachOption::FILL, sfg::Table::AttachOption::FILL);
	}
}

