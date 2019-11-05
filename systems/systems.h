#pragma once
#include "ecs.h"
#include "components.h"

namespace ecs
{
	enum class System_type : size_t { Renderer, Controller, Movement, Facing, AI, Animation, Attributes };
}

namespace systems
{
	void add_systems (ecs::System_manager* mgr);
}