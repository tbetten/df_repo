#ifdef NOT_USED
#include "entity_at.h"
#include "components.h"
#include "position.h"

#include <algorithm>

std::vector<ecs::Entity_id> get_entity_at(ecs::Entity_manager* mgr, std::string map, Position::Layer layer, sf::Vector2i coords)
{
	std::vector<ecs::Entity_id> result;
	auto position_comp = mgr->get_component<ecs::Component<Position>>(ecs::Component_type::Position);
	auto data = position_comp->m_data;
	//auto itr = std::cbegin(data);
	auto equal_position = [map, layer, coords](Position p) {return p.current_map == map && p.layer == layer && p.coords == coords; };
	for (auto itr = std::find_if(std::cbegin(data), std::cend(data), [map, layer, coords](Position p) {return p.current_map == map && p.layer == layer && p.coords == coords; }); itr != std::cend(data); itr = std::find_if(itr, std::cend(data), [map, layer, coords](Position p) {return p.current_map == map && p.layer == layer && p.coords == coords; }))
	{
		auto index = std::distance(std::cbegin(data), itr);
		auto opt_entity = mgr->idex_to_entity(ecs::Component_type::Position, index);
		if (opt_entity) result.push_back(opt_entity.value());
		itr++;
	}
	return result;
}
#endif