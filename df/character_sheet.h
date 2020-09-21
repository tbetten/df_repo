#pragma once
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <optional>
#include <any>
#include "ecs_types.h"
#include "components.h"
#include "attributes.h"

struct Shared_context;

namespace ecs
{
	template<typename T>
	struct Component;
}

struct Character;
struct Attributes;
struct Container;
struct Drawable;
//enum class Attribute : unsigned int;

struct Party_member
{
	Party_member (ecs::Entity_id entity, ecs::Entity_manager* em);

	std::string name = "";
	ecs::Entity_id id = 0;
	ecs::Entity_manager* m_em;
	Character* character = nullptr;
	Attributes* attributes = nullptr;
	Container* container = nullptr;
	Drawable* drawable = nullptr;
	sf::Vector2i coords;
//	sfg::Widget::Ptr character_view = nullptr;
//	sfg::Widget::Ptr attributes_view = nullptr;
//	sfg::Widget::Ptr inventory_view = nullptr;
};

struct Inventory_item
{
	sfg::ToggleButton::Ptr button;
	bool equipped;
	std::optional<ecs::Entity_id> entity;
};

class Character_sheet
{
public:
	enum class Sheet_view { Character_view, Attributes_view, Inventory_view };
	using Itr = std::vector<Party_member>::iterator;

	explicit Character_sheet(Shared_context* context);
	void populate_party ();
	void fill_data(ecs::Entity_id entity);
	sfg::Widget::Ptr get_charsheet() const;
	void on_select ();
private:
//	std::vector<Party_member> get_party_data() const;
	void add_sheets (Party_member& member);
	sfg::Widget::Ptr create_character_page(const Party_member& m) const;
	sfg::Widget::Ptr create_attribute_page(const Party_member& m) const;
	sfg::Widget::Ptr create_inventory_page(const Party_member& m);


	void on_inventory_changed (const std::any& val);
	
	void on_drop_item();
	void on_equip();
	void on_use_item();
	

	sfg::Widget::Ptr m_root;
	ecs::Component<Character>* m_character;
	ecs::Component<Attributes>* m_attributes;
	ecs::Component<Container>* m_container;
	Shared_context* m_context;
	ecs::Entity_manager* m_em;
	ecs::Entity_id m_current_member;
	std::vector<Party_member> m_party;
	std::vector<Inventory_item> m_inventory;
	std::map<Sheet_view, sfg::Notebook::IndexType> m_sheet_indices;
};

void populate_character_page (const Party_member& m);
void populate_attribute_page (const Party_member& m);
void populate_inventory_page (const Party_member& m);
std::string get_attribute (Attributes* attribs, attributes::Attrib attrib);