#include "components.h"

#include "facing.h"
#include "item.h"
#include "container.h"
#include "animation.h"
#include "position.h"
#include "drawable.h"
#include "character.h"
#include "attributes.h"
#include "tile_type.h"
#include "lightsource.h"
#include "sensor.h"
#include "reactor.h"
#include "armour.h"
#include "inventory.h"
#include "equippable.h"
#include "skill_comp.h"

void comps::add_components (ecs::Entity_manager* mgr)
{
	mgr->add_component (ecs::Component_type::Position, std::unique_ptr<ecs::C_base> (new  ecs::Component<Position> ()));
	mgr->add_component (ecs::Component_type::Drawable, std::unique_ptr<ecs::C_base> (new ecs::Component<Drawable> ()));
	mgr->add_component (ecs::Component_type::Facing, std::unique_ptr<ecs::C_base> (new ecs::Component<Facing> ()));
	mgr->add_component(ecs::Component_type::Character, std::unique_ptr<ecs::C_base>(new ecs::Component<Character>()));
	mgr->add_component(ecs::Component_type::Item, std::unique_ptr<ecs::C_base>(new ecs::Component<Item>()));
	mgr->add_component(ecs::Component_type::Container, std::unique_ptr<ecs::C_base>(new ecs::Component <Container>()));
	mgr->add_component(ecs::Component_type::Attributes, std::unique_ptr<ecs::C_base>(new ecs::Component<Attributes>()));
	mgr->add_component(ecs::Component_type::Animation, std::unique_ptr<ecs::C_base>(new ecs::Component<Animation>()));
	mgr->add_component(ecs::Component_type::Tile_type, std::unique_ptr<ecs::C_base>(new ecs::Component<Tile_type>()));
	mgr->add_component(ecs::Component_type::Lightsource, std::unique_ptr<ecs::C_base>(new ecs::Component<Lightsource>()));
	mgr->add_component(ecs::Component_type::Sensor, std::unique_ptr<ecs::C_base>(new ecs::Component<Sensor>()));
	mgr->add_component(ecs::Component_type::Reactor, std::unique_ptr<ecs::C_base>(new ecs::Component<Reactor>()));
	mgr->add_component (ecs::Component_type::Armour, std::unique_ptr<ecs::C_base> (new ecs::Component<Armour> ()));
	mgr->add_component (ecs::Component_type::Inventory, std::unique_ptr<ecs::C_base> (new ecs::Component<Inventory> ()));
	mgr->add_component (ecs::Component_type::Equippable, std::unique_ptr<ecs::C_base> (new ecs::Component<Equippable> ()));
	mgr->add_component (ecs::Component_type::Skill, std::unique_ptr<ecs::C_base> (new ecs::Component<Skill_comp> ()));
}


