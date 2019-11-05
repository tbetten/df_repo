#pragma once
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include "ecs_types.h"
#include "components.h"
#include "character.h"
#include "attributes.h"

struct Shared_context;

struct Party_member
{
	Party_member() :name{ "" }, id{ 0 }, character{ nullptr }, attributes{ nullptr }{}
	std::string name;
	ecs::Entity_id id;
	Character* character;
	Attributes* attributes;
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
	void on_select() const;
	std::string get_attribute(Attributes* attribs, Attribute attrib) const;

	sfg::Widget::Ptr m_root;
	ecs::Component<Character>* m_character;
	ecs::Component<Attributes>* m_attributes;
	Shared_context* m_context;
	ecs::Entity_manager* m_em;
	std::vector<Party_member> m_party;
};