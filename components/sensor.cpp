#include "sensor.h"
#include "property.h"

void Sensor::reset()
{
	active = true;
	state = false;
}

Sensor::Trigger convert_trigger (std::string_view trigger)
{
	if (trigger == "use") return Sensor::Trigger::Use;
	if (trigger == "use_with_item") return Sensor::Trigger::Use_with_item;
	if (trigger == "enter") return Sensor::Trigger::Enter;
	return Sensor::Trigger::Invalid;
}

void Sensor::load (std::span<tiled::Property> props)
{
	active = tiled::get_property_value<bool> (props, "active").value ();
	id = tiled::get_property_value<int> (props, "object_id").value ();
	state = tiled::get_property_value<bool> (props, "state").value ();
	trigger = convert_trigger (tiled::get_property_value<std::string> (props, "activation").value ());
	item = tiled::get_property_value<std::string> (props, "item_handle").value_or ("");
}