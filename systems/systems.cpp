#include "systems.h"
#include "renderer.h"
#include "movement.h"
#include "controller.h"
#include "facing_system.h"

void systems::add_systems (ecs::System_manager* mgr)
{
	mgr->add_system (ecs::System_type::Renderer, std::unique_ptr<ecs::S_base> (new Renderer (ecs::System_type::Renderer, mgr)));
	mgr->add_system (ecs::System_type::Controller, std::unique_ptr<ecs::S_base> (new Controller (ecs::System_type::Controller, mgr)));
	mgr->add_system (ecs::System_type::Movement, std::unique_ptr<ecs::S_base> (new Movement (ecs::System_type::Movement, mgr)));
	mgr->add_system (ecs::System_type::Facing, std::unique_ptr<ecs::S_base> (new Facing_system (ecs::System_type::Facing, mgr)));
}