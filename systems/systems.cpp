#include "systems.h"
#include "renderer.h"
#include "movement.h"
#include "controller.h"
#include "facing_system.h"
#include "animation_system.h"
#include "attribute_system.h"
#include "icon_composer.h"
#include "ai.h"
#include "lighting.h"
#include "inventory_system.h"
#include "interaction.h"
#include "encumbrance.h"
#include "cursor_system.h"
#include "skill_system.h"

namespace messaging
{
	class Messenger;
}

void systems::add_systems (ecs::System_manager* mgr)
{
	auto m = mgr->get_messenger();
	mgr->add_system (ecs::System_type::Renderer, std::unique_ptr<ecs::S_base> (new Renderer (ecs::System_type::Renderer, mgr, m)));
	mgr->add_system (ecs::System_type::Controller, std::unique_ptr<ecs::S_base> (new Controller (ecs::System_type::Controller, mgr, m)));
	mgr->add_system (ecs::System_type::Movement, std::unique_ptr<ecs::S_base> (new Movement (ecs::System_type::Movement, mgr, m)));
	mgr->add_system (ecs::System_type::Facing, std::unique_ptr<ecs::S_base> (new Facing_system (ecs::System_type::Facing, mgr, m)));
	mgr->add_system(ecs::System_type::AI, std::unique_ptr<ecs::S_base>(new AI(ecs::System_type::AI, mgr, m)));
	mgr->add_system(ecs::System_type::Animation, std::unique_ptr<ecs::S_base>(new Animation_system(ecs::System_type::Animation, mgr)));
	mgr->add_system(ecs::System_type::Attributes, std::unique_ptr<ecs::S_base>(new Attribute_system(ecs::System_type::Attributes, mgr)));
	mgr->add_system(ecs::System_type::Icon_composer, std::unique_ptr<ecs::S_base>(new Icon_composer(ecs::System_type::Icon_composer, mgr, m)));
	mgr->add_system(ecs::System_type::Lighting, std::unique_ptr<ecs::S_base>(new Lighting(ecs::System_type::Lighting, mgr, m)));
	mgr->add_system(ecs::System_type::Inventory, std::unique_ptr<ecs::S_base>(new Inventory_system(ecs::System_type::Inventory, mgr, m)));
	mgr->add_system(ecs::System_type::Interaction, std::unique_ptr<ecs::S_base>(new Interaction(ecs::System_type::Interaction, mgr, m)));
	mgr->add_system(ecs::System_type::Encumbrance, std::unique_ptr<ecs::S_base>(new Encumbrance(ecs::System_type::Encumbrance, mgr, m)));
	mgr->add_system (ecs::System_type::Cursor, std::unique_ptr<ecs::S_base> (new Cursor_system (ecs::System_type::Cursor, mgr, m)));
	mgr->add_system (ecs::System_type::Skill, std::unique_ptr<ecs::S_base> (new Skill_system (ecs::System_type::Skill, mgr, m)));
}