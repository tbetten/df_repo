#include "map_data.h"

std::vector<ecs::Entity_id> Map_data::get_entities_at(int layer, unsigned int tile_index)
{
	std::vector <ecs::Entity_id> result;
	auto tile = layers[layer][tile_index];
	for (auto entity : tile)
	{
		if (entity)
		{
			result.push_back(*entity);
		}
	}
	return result;
}

std::vector<std::vector<ecs::Entity_id>> Map_data::get_entities_at(unsigned int tile_index)
{
	std::vector<std::vector<ecs::Entity_id>> result;
	if (tile_index < 0 || tile_index > layers[0].size()) return result;
	for (int i = 0; i < layers.size(); ++i)
	{
		result.emplace_back(get_entities_at(i, tile_index));
	}
	return result;
}