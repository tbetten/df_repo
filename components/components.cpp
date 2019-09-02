#include "components.h"

#include "facing.h"
#include "item.h"
#include "container.h"
#include "animation.h"
#include "position.h"
#include "drawable.h"
#include "character.h"
#include "attributes.h"

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
}


