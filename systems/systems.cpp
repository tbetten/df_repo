#include "systems.h"
#include "renderer.h"
#include "movement.h"
#include "controller.h"
#include "facing_system.h"
#include "animation_system.h"
#include "ai.h"

void systems::add_systems (ecs::System_manager* mgr)
{
	mgr->add_system (ecs::System_type::Renderer, std::unique_ptr<ecs::S_base> (new Renderer (ecs::System_type::Renderer, mgr)));
	mgr->add_system (ecs::System_type::Controller, std::unique_ptr<ecs::S_base> (new Controller (ecs::System_type::Controller, mgr)));
	mgr->add_system (ecs::System_type::Movement, std::unique_ptr<ecs::S_base> (new Movement (ecs::System_type::Movement, mgr)));
	mgr->add_system (ecs::System_type::Facing, std::unique_ptr<ecs::S_base> (new Facing_system (ecs::System_type::Facing, mgr)));
	mgr->add_system(ecs::System_type::AI, std::unique_ptr<ecs::S_base>(new AI(ecs::System_type::AI, mgr)));
	mgr->add_system(ecs::System_type::Animation, std::unique_ptr<ecs::S_base>(new Animation_system(ecs::System_type::Animation, mgr)));
}