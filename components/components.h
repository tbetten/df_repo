#pragma once
#include "component.h"


#include <memory>



namespace ecs
{
	enum class Component_type : size_t { Position, Drawable, Facing, Character, Item, Container, Attributes, Animation, Tile_type };
}


struct Tilemap;
namespace comps
{
	void add_components (ecs::Entity_manager* mgr);
}

/*namespace db
{
	class db_connection;
	class prepared_statement;
	using db_connection_ptr = std::unique_ptr<db_connection>;
	using prepared_statement_ptr = std::shared_ptr<prepared_statement>;
}*/

