#include "drawable.h"
#include "component.h"
#include "components.h"
#include "resource_cache.h"
#include "grid.h"

void Drawable::reset () {}

void init_drawable (ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache)//, grid::Layout layout)
{
	auto res = cache->get_obj (key);
	auto data = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	data->texture = *cache::get_val<sf::Texture> (res.get ());
	data->sprite.setTexture (data->texture);
	data->sprite.setOrigin (static_cast<sf::Vector2f> (data->texture.getSize () / 2u));
//	data->layout = layout;
}