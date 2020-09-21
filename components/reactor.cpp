#include "reactor.h"
#include <span>
#include "property.h"

void Reactor::reset() {}

Reactor::On_trigger convert_action (const std::string_view action)
{
	if (action == "activate") return Reactor::On_trigger::Activate;
	if (action == "change_tile") return Reactor::On_trigger::Change_tile;
	return Reactor::On_trigger::No_action;
}

void Reactor::load (std::span<tiled::Property> props)
{
	
	triggered_by = tiled::get_property_value<int> (props, "triggered_by").value ();
	tile_true = tiled::get_property_value<std::string> (props, "tile_true").value_or ("");
	tile_false = tiled::get_property_value<std::string> (props, "tile_false").value_or ("");
	make_accessible = tiled::get_property_value<bool> (props, "make_accessible").value_or (false);
	make_transparant = tiled::get_property_value<bool> (props, "make_transparant").value_or (false);
	action = convert_action (tiled::get_property_value<std::string> (props, "on_trigger").value_or ("no_action"));
	entity = tiled::get_property_value<size_t> (props, "entity").value_or (0);
	id = tiled::get_property_value<int> (props, "object_id").value ();
}