#pragma once
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <optional>
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
//enum class Attribute : unsigned int;

struct Party_member
{
	std::string name = "";
	ecs::Entity_id id = 0;
	Character* character = nullptr;
	Attributes* attributes = nullptr;
	Container* container = nullptr;
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
	explicit Character_sheet(Shared_context* context);
	void fill_data(ecs::Entity_id entity);
	sfg::Widget::Ptr get_charsheet() const;
private:
	std::vector<Party_member> get_party_data() const;
	Party_member get_party_member(ecs::Entity_id entity) const;
	sfg::Widget::Ptr create_character_page() const;
	sfg::Widget::Ptr create_attribute_page() const;
	sfg::Widget::Ptr create_inventory_page() const;
	void on_select();
	std::string get_attribute(Attributes* attribs, Attribute attrib) const;

	sfg::Widget::Ptr m_root;
	ecs::Component<Character>* m_character;
	ecs::Component<Attributes>* m_attributes;
	ecs::Component<Container>* m_container;
	Shared_context* m_context;
	ecs::Entity_manager* m_em;
	std::vector<Party_member> m_party;
	std::vector<Inventory_item> m_inventory;
};