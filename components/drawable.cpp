#include "drawable.h"
#include "component.h"
#include "components.h"
#include "resource_cache.h"
#include "facing_payload.h"
#pragma warning (push)
#pragma warning (disable : 26812)
#include "tinyxml2.h"
#pragma warning (pop)

Drawable::Drawable() : locations{"base_icon", "beard", "facing_indicator"}
{
	composed_icon = std::make_unique<sf::RenderTexture>();
	composed_icon->create(32, 32);
	for (auto location : locations)
	{
		icon_parts[location] = Icon{};
	}
}
void Drawable::reset () {}

void fill_icon(Icon& icon, std::string key, cache::Cache* cache)
{
	icon.key = std::move(key);
	icon.resource_ptr = cache->get_obj(icon.key);
	icon.texture = cache::get_val<sf::Texture>(icon.resource_ptr.get());
	icon.sprite.setTexture(*icon.texture);
}

void clear_icon(Icon& icon)
{
	icon.key = "";
	icon.resource_ptr.reset();
	icon.texture = nullptr;
}

void init_drawable(ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache, const sf::IntRect& rect)
{
	auto data = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	auto& base_icon = data->icon_parts.at("base_icon");
	fill_icon(base_icon, key, cache);
	mgr->get_messenger()->notify("icon_changed", entity);
	auto& tex = data->composed_icon->getTexture();
	data->sprite.setTexture(tex);
	data->sprite.setOrigin(static_cast<sf::Vector2f> (tex.getSize() / 2u));
}



void fill_icon_part(ecs::Entity_manager* mgr, cache::Cache* cache, std::string key, const std::string& location, ecs::Entity_id entity)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	fill_icon(icon_part, key, cache);
	mgr->get_messenger()->notify("icon_changed", entity);
}

void fill_icon_part(ecs::Entity_manager* mgr, std::shared_ptr<cache::Resource_base> texture_resource, sf::IntRect area, const std::string& location, ecs::Entity_id entity)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	icon_part.resource_ptr = texture_resource;
	icon_part.texture = cache::get_val<sf::Texture>(icon_part.resource_ptr.get());
	icon_part.sprite.setTexture(*icon_part.texture);
	icon_part.sprite.setTextureRect(area);
	mgr->get_messenger()->notify("icon_changed", entity);
}

void clear_icon_part(ecs::Entity_manager* mgr, const std::string& location, ecs::Entity_id entity)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	auto& icon_part = data->icon_parts.at(location);
	clear_icon(icon_part);
	mgr->get_messenger()->notify("icon_changed", entity);
}